#pragma once

#include <Core.hpp>
#include <magic_enum.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/optional.hpp>
#include <cereal_optional_nvp.h>

enum class AssetType {
	UNKNOWN,
	MATERIAL,
	TEXT,
	TEXTURE_2D,
	MESH
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
	bool isDirty = false;
};

constexpr AssetType ExtensionToAssetType(const std::string& ext) {
	if (ext == ".mat") {
		return AssetType::MATERIAL;
	}
	if (ext == ".txt") {
		return AssetType::TEXT;
	}
	if (ext == ".png") {
		return AssetType::TEXTURE_2D;
	}
	if (std::set<std::string>{ ".obj", ".fbx" }.contains(ext)) {
		return AssetType::MESH;
	}
	return AssetType::UNKNOWN;
}