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

struct MeshAsset : public Asset {
	MeshAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MESH; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		vertices.clear();
		indices.clear();

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.lexically_normal().string(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs);

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
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
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
		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
		aiFace face = mesh.mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}  

	// process material
	if (mesh.mMaterialIndex >= 0) {
		
	}
}