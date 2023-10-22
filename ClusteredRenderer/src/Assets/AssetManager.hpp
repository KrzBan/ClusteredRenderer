#pragma once

#include <Core.hpp>
#include <UUID.hpp>

enum class AssetType {
	UNKNOWN,
	MATERIAL,
	TEXT
};

constexpr std::string AssetTypeToString(AssetType type) {
	switch (type) {
	case AssetType::UNKNOWN:
		return "Unknown";
	case AssetType::MATERIAL:
		return "Material";
	case AssetType::TEXT:
		return "Text";
	}

	throw std::invalid_argument("Invalid enum value");
}


struct AssetInfo {
	std::filesystem::path path;
	AssetType assetType;
};

struct AssetManager {
public:
	static void Init(const std::string& basePath);

	static const std::unordered_map<kb::UUID, AssetInfo>& GetManagedAssets();
	static const std::unordered_set<std::filesystem::path>& GetUnmanagedAssets();

private:
	static void DiscoverAssets(const std::string& basePath);
};