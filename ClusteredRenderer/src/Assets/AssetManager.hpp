#pragma once

#include <Core.hpp>
#include <UUID.hpp>

#include "AssetWatcher.hpp"

enum class AssetType {
	UNKNOWN,
	MATERIAL,
	TEXT,
	TEXTURE_2D
};

constexpr std::string AssetTypeToString(AssetType type) {
	switch (type) {
	case AssetType::UNKNOWN:
		return "Unknown";
	case AssetType::MATERIAL:
		return "Material";
	case AssetType::TEXT:
		return "Text";
	case AssetType::TEXTURE_2D:
		return "Texture 2D";
	}

	throw std::invalid_argument("Invalid enum value");
}


struct AssetInfo {
	AssetType assetType;
	std::filesystem::file_time_type lastWrite;
};

class MetaData;

struct AssetManager {
public:
	static void Init(const std::string& basePath);

	static void HandleFileChanges(const std::vector<FileActions>& queue);

	static const std::unordered_map<kb::UUID, AssetInfo>& GetManagedAssets();
	static const std::unordered_set<std::filesystem::path>& GetUnmanagedAssets();

	static std::filesystem::path GetPath(kb::UUID);

	static void Clear();

private:
	static void DiscoverAssets(const std::string& basePath);

	static void AddFile(const std::filesystem::path& path);

	static std::optional<MetaData> FetchStrayMetaData(std::filesystem::path path);
	static std::optional<MetaData> FetchFileMetaData(std::filesystem::path path);
};