#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 biTangent;
	glm::vec2 texCoords;
};

struct Submesh {
	std::string name;
	kb::UUID submeshId;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct MeshAsset : public Asset {
	MeshAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MESH; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		submeshes.clear();

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.lexically_normal().string(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
				/* aiProcess_FlipUVs | */ // We Flip by default when loading texture
			aiProcess_PreTransformVertices |
			aiProcess_GenSmoothNormals);

		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			spdlog::error("[MeshAsset::LoadAsset(): {}", importer.GetErrorString());
			return;
		}

		ProcessNode(*scene->mRootNode, *scene);
	}

	virtual void LoadMeta(cereal::JSONInputArchive& archive) override {
		
	};
	virtual void SaveMeta(cereal::JSONOutputArchive& archive) const override {
		
	};

private:
	void ProcessNode(aiNode& node, const aiScene& scene);
	void ProcessMesh(aiMesh& mesh, const aiScene& scene);

public:
	std::vector<Submesh> submeshes;
};

template <>
inline AssetType AssetTypeFromType<MeshAsset>() {
	return AssetType::MESH;
}

inline void MeshAsset::ProcessNode(aiNode& node, const aiScene& scene) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node.mNumMeshes; i++) {
		aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
		ProcessMesh(*mesh, scene);
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node.mNumChildren; i++) {
		ProcessNode(*node.mChildren[i], scene);
	}
}

inline void MeshAsset::ProcessMesh(aiMesh& mesh, const aiScene& scene) {
	Submesh submesh;
	submesh.name = mesh.mName.C_Str();
	submesh.vertices.reserve(mesh.mNumVertices);
	for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
		Vertex vertex{};
		vertex.position		= { mesh.mVertices[i].x,	mesh.mVertices[i].y,	mesh.mVertices[i].z };
		vertex.normal		= { mesh.mNormals[i].x,		mesh.mNormals[i].y,		mesh.mNormals[i].z }; 
		vertex.tangent		= { mesh.mTangents[i].x,	mesh.mTangents[i].y,	mesh.mTangents[i].z };
		vertex.biTangent	= { mesh.mBitangents[i].x,	mesh.mBitangents[i].y,	mesh.mBitangents[i].z };

		if (mesh.mTextureCoords[0]){
			vertex.texCoords = { mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y };
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f); 
		}
		submesh.vertices.push_back(vertex);
	}
	
	submesh.vertices.reserve(mesh.mNumFaces * 3);	// Minimal space needed, potential resizes
	for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
		aiFace face = mesh.mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			submesh.indices.push_back(face.mIndices[j]);
	}  

	// process material
	if (mesh.mMaterialIndex >= 0) {
		
	}

	submeshes.push_back(std::move(submesh));
}