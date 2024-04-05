#pragma once

#include <Core.hpp>
#include <UUID.hpp>

enum class AssetType {
	UNKNOWN,
	MATERIAL,
	TEXT,
	TEXTURE_2D,
	MESH,
	SHADER_SOURCE,
	SHADER
};

template<class Asset>
inline AssetType AssetTypeFromType() {
	return AssetType::UNKNOWN;
}

class Asset {
public:
	// Making Type par of Asset<Type> signature, doesn't let us use this as common base class
	virtual constexpr AssetType GetType() const { return AssetType::UNKNOWN; };

	virtual void LoadMeta(cereal::JSONInputArchive& archive) {};
	virtual void SaveMeta(cereal::JSONOutputArchive& archive) const{};

	virtual void LoadAsset(const std::filesystem::path& path) {}
	virtual void SaveAsset(const std::filesystem::path& path) const {}

	virtual ~Asset() = default;

public:
	bool isDirty = true;	// For Renderer only
	kb::UUID assetId{ 0 };
};

constexpr AssetType ExtensionToAssetType(const std::string& ext) {
	if (ext == ".mat") {
		return AssetType::MATERIAL;
	}
	if (ext == ".txt") {
		return AssetType::TEXT;
	}
	if (std::set<std::string>{ ".shader" }.contains(ext)) {
		return AssetType::SHADER;
	}
	if (std::set<std::string>{ ".png", ".jpg", ".jpeg", ".hdr", ".tga" }.contains(ext)) {
		return AssetType::TEXTURE_2D;
	}
	if (std::set<std::string>{ ".obj", ".fbx", ".gltf", ".usda" }.contains(ext)) {
		return AssetType::MESH;
	}
	if (std::set<std::string>{ ".vert", ".tesc", ".tese", ".geom", ".frag", ".comp" }.contains(ext)) {
		return AssetType::SHADER_SOURCE;
	}
	if (std::set<std::string>{ ".shader" }.contains(ext)) {
		return AssetType::SHADER;
	}
	return AssetType::UNKNOWN;
}