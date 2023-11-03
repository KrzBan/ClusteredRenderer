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

	std::unordered_map<kb::UUID, std::filesystem::path> idToPath;
	std::unordered_map<std::filesystem::path, kb::UUID> pathToId;

	std::unordered_set<std::filesystem::path> unmanagedAssets;
	std::unordered_map<std::filesystem::path, MetaData> strayMetaFiles;
};

static AssetManagerData s_AssetManagerData;

const std::unordered_map<kb::UUID, AssetInfo>& AssetManager::GetManagedAssets() {
	return s_AssetManagerData.managedAssets;
}
const std::unordered_set<std::filesystem::path>& AssetManager::GetUnmanagedAssets() {
	return s_AssetManagerData.unmanagedAssets;
}

std::filesystem::path AssetManager::GetPath(kb::UUID id) {
	if (s_AssetManagerData.idToPath.contains(id)) {
		return s_AssetManagerData.idToPath[id];
	}
	return {};
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
		
		AddFile(path);
	}
}

void AssetManager::HandleFileChanges(const std::vector<FileActions>& queue) {

	using namespace std;

	for (const auto& action : queue) {
		const auto path = filesystem::path(action.directory) / filesystem::path(action.filename);
		auto pathMeta = path; pathMeta += filesystem::path(".meta");

		// Ignore directories
		if (filesystem::is_directory(path)) {
			continue;
		}

		// Ignore .meta files
		if (path.extension().compare(".meta") == 0) {
			continue;
		}

		switch (action.action) {
		case efsw::Actions::Add: {
			AddFile(path);
			break;
		}
			
		case efsw::Actions::Delete: {
			// Remove from registers
			if (s_AssetManagerData.pathToId.contains(path)) {
				const auto fileId = s_AssetManagerData.pathToId[path];
				s_AssetManagerData.managedAssets.erase(fileId);
				s_AssetManagerData.idToPath.erase(fileId);
				s_AssetManagerData.pathToId.erase(path);
			}
			if (s_AssetManagerData.unmanagedAssets.contains(path)) {
				s_AssetManagerData.unmanagedAssets.erase(path);
			}
			
			if (std::filesystem::exists(pathMeta)) {
				MetaData strayMetaFile = MetaData::ReadMetaFile(pathMeta);
				s_AssetManagerData.strayMetaFiles[path.filename()] = strayMetaFile;
				filesystem::remove(pathMeta);
				spdlog::debug("Saved MetaData of {}", pathMeta);
			}
			break;
		}
		case efsw::Actions::Modified: {
			// Update timestamp
			const auto lastModified = std::filesystem::last_write_time(path);
			// Update register timestamp
			if (s_AssetManagerData.pathToId.contains(path)) {
				const auto fileId = s_AssetManagerData.pathToId[path];
				s_AssetManagerData.managedAssets[fileId].lastWrite = lastModified;
			}
			// Update .meta timestamp
			if (std::filesystem::exists(pathMeta)) {
				MetaData metaData = MetaData::ReadMetaFile(pathMeta);
				metaData.lastModified = lastModified;
				MetaData::WriteMetaFile(pathMeta, metaData);
				spdlog::debug("Updated MetaData of {}", pathMeta);
			}

			// Reload assset

			break;
		}
		case efsw::Actions::Moved: {
			// Move registries
			const auto oldPath = filesystem::path(action.directory) / filesystem::path(action.oldFilename);

			if (s_AssetManagerData.pathToId.contains(oldPath)) {
				const auto fileId = s_AssetManagerData.pathToId[oldPath];
				s_AssetManagerData.idToPath.erase(fileId);
				s_AssetManagerData.pathToId.erase(oldPath);

				s_AssetManagerData.idToPath[fileId] = path;
				s_AssetManagerData.pathToId[path] = fileId;
			}
			if (s_AssetManagerData.unmanagedAssets.contains(oldPath)) {
				s_AssetManagerData.unmanagedAssets.erase(oldPath);
				s_AssetManagerData.unmanagedAssets.insert(path);
			}

			// Move .meta file
			auto oldPathMeta = oldPath;
			oldPathMeta += filesystem::path(".meta");
			if (std::filesystem::exists(oldPathMeta)) {
				if (std::filesystem::exists(pathMeta)) {
					spdlog::error("Tried renaming {} to {}, but the file already exists", oldPathMeta, pathMeta);
					break;
				}
				filesystem::rename(oldPathMeta, pathMeta);
			}
			break;
		}
		default:
			std::cout << "Should never happen!" << std::endl;
		}
	}
}

void AssetManager::AddFile(const std::filesystem::path& path) {

	auto extensionStr = path.extension().string();

	if (extensionStr == ".meta")
		return;

	auto assetType = ExtensionToAssetType(extensionStr);

	if (assetType == AssetType::UNKNOWN) {
		s_AssetManagerData.unmanagedAssets.insert(path);
		return;
	}

	auto metaPath = path;
	metaPath += std::filesystem::path{ ".meta" };

	MetaData metaData{};
	if (const auto strayMetaData = FetchStrayMetaData(path); strayMetaData) {
		metaData = strayMetaData.value();
		MetaData::WriteMetaFile(metaPath, metaData);
	}
	else if (const auto loadedMetaData = FetchFileMetaData(path); loadedMetaData) {
		metaData = loadedMetaData.value();
	}
	else {
		metaData.lastModified = std::filesystem::last_write_time(path);
		MetaData::WriteMetaFile(metaPath, metaData);
	}

	AssetInfo info{ assetType, metaData.lastModified };
	kb::UUID fileId{ metaData.assetID };
	s_AssetManagerData.managedAssets[fileId] = info;

	s_AssetManagerData.idToPath[fileId] = path;
	s_AssetManagerData.pathToId[path] = fileId;
}

std::optional<MetaData> AssetManager::FetchStrayMetaData(std::filesystem::path path) {
	const auto filename = path.filename();
	if (not s_AssetManagerData.strayMetaFiles.contains(filename))
		return {};

	auto metaPath = path; metaPath += ".meta";
	auto lastModified = std::filesystem::last_write_time(path);

	MetaData metaData = s_AssetManagerData.strayMetaFiles.at(filename);

	if (std::filesystem::exists(metaPath)) {
		spdlog::error("Stray .meta data available, but file {} already exists", metaPath);
		return {};
	}

	if (lastModified != metaData.lastModified) {
		spdlog::error("LastModified timestamp mismatch when fetching stray .meta data for path {}", path);
		return {};
	}

	s_AssetManagerData.strayMetaFiles.erase(path);
	return metaData;
}

std::optional<MetaData> AssetManager::FetchFileMetaData(std::filesystem::path path) {
	auto metaPath = path;
	metaPath += ".meta";

	if (not std::filesystem::exists(metaPath))
		return {};
	
	MetaData loadedMetaData = MetaData::ReadMetaFile(metaPath);
	if (loadedMetaData.lastModified != std::filesystem::last_write_time(path)) {
		spdlog::error("LastModified timestamp mismatch when loading existing .meta file");

		spdlog::info("Removing {}", metaPath);
		std::filesystem::remove(metaPath);

		return {};
	}

	return loadedMetaData;
}