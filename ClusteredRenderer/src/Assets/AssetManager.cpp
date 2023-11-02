#include "AssetManager.hpp"

#include "MaterialAsset.hpp"
#include "TextAsset.hpp"
#include "FileUtils.hpp"

#include "AssetWatcher.hpp"
#include "MetaData.hpp"

#include <cereal/archives/json.hpp>

using Asset = std::variant<MaterialAsset, TextAsset>;

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

void AssetManager::Clear() {
	s_AssetManagerData.managedAssets.clear();
	s_AssetManagerData.unmanagedAssets.clear();
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
		auto extensionStr = path.extension().string();

		if (extensionStr == ".meta")
			continue;

		auto assetType = ExtensionToAssetType(extensionStr);

		if (assetType == AssetType::UNKNOWN) {
			s_AssetManagerData.unmanagedAssets.insert(path);
			continue;
		}

		auto metaPath = path;
		metaPath += std::filesystem::path{ ".meta" };

		MetaData metaData{};
		if (std::filesystem::exists(metaPath)) {
			metaData = MetaData::ReadMetaFile(metaPath);
		}
		else {
			metaData.lastModified = std::filesystem::last_write_time(path);
			MetaData::WriteMetaFile(metaPath, metaData);
		}

		AssetInfo info{ path, assetType, metaData.lastModified };
		kb::UUID fileId{ metaData.assetID };
		s_AssetManagerData.managedAssets[fileId] = info;
	}
}

void AssetManager::HandleFileChanges(const std::vector<FileActions>& queue) {

	for (const auto& action : queue) {
		switch (action.action) {
		case efsw::Actions::Add:
			spdlog::debug("DIR ({}) FILE ({}) has event Added", action.directory, action.filename);
			break;
		case efsw::Actions::Delete:
			spdlog::debug("DIR ({}) FILE ({}) has event Delete", action.directory, action.filename);
			break;
		case efsw::Actions::Modified:
			spdlog::debug("DIR ({}) FILE ({}) has event Modified", action.directory, action.filename);
			break;
		case efsw::Actions::Moved:
			spdlog::debug("DIR ({}) FILE ({}) has event Moved from ({})",
				action.directory, action.filename, action.oldFilename);
			break;
		default:
			std::cout << "Should never happen!" << std::endl;
		}
	}
	
}