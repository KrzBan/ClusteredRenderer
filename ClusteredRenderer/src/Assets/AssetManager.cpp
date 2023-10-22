#include "AssetManager.hpp"

#include "MaterialAsset.hpp"
#include "TextAsset.hpp"
#include "FileUtils.hpp"

#include <cereal/archives/json.hpp>

using Asset = std::variant<MaterialAsset, TextAsset>;

constexpr AssetType ExtensionToAssetType(const std::string& ext) {
	if (ext == ".mat") {
		return AssetType::MATERIAL;
	}
	if (ext == ".txt") {
		return AssetType::TEXT;
	}
	return AssetType::UNKNOWN;
}

constexpr std::string AssetToString(const Asset& asset) {
	std::string name = std::visit(overload(
		[](const MaterialAsset& material) { return "Material"; },
		[](const TextAsset& text) { return "Text"; }
	), asset);

	return name;
}

struct AssetManagerData {
	std::unordered_map<std::string, MaterialAsset> materialAsset;
	std::unordered_map<std::string, TextAsset> textAssets;

	std::unordered_map<kb::UUID, AssetInfo> managedAssets;
	std::unordered_set<std::filesystem::path> unmanagedAssets;
};

static AssetManagerData s_AssetManagerData;

const std::unordered_map<kb::UUID, AssetInfo>& AssetManager::GetManagedAssets() {
	return s_AssetManagerData.managedAssets;
}
const std::unordered_set<std::filesystem::path>& AssetManager::GetUnmanagedAssets() {
	return s_AssetManagerData.unmanagedAssets;
}

void AssetManager::Init(const std::string& basePath) {
	DiscoverAssets(basePath);
}

void AssetManager::DiscoverAssets(const std::string& basePath) {
	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

	for (const auto& dirEntry : recursive_directory_iterator(basePath)) {
		if (dirEntry.is_directory())
			continue;

		auto path = dirEntry.path();
		auto pathStr = path.string();
		auto extension = path.extension().string();

		auto assetType = ExtensionToAssetType(extension);

		if (assetType != AssetType::UNKNOWN) {

			std::ifstream f(path, std::ios::in);

			try {
				cereal::JSONInputArchive iarchive(f);

				uint64_t uuid{ 0 };
				iarchive(uuid);

				AssetInfo info{ path, assetType };
				kb::UUID fileId{ uuid };
				s_AssetManagerData.managedAssets[fileId] = info;
				continue;
			}
			catch (...) {
				// Can't decode, not JSON
			}
		}

		s_AssetManagerData.unmanagedAssets.insert(path);
	}
}