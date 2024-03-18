#include "AssetManager.hpp"

#include "AssetTypes/AssetType.hpp"

#include "FileUtils.hpp"
#include "CommonMetaData.hpp"

#include <cereal/archives/json.hpp>

struct AssetManagerData {
	std::unordered_map<kb::UUID, std::filesystem::path> idToPath;
	std::unordered_map<std::filesystem::path, kb::UUID> pathToId;

	std::unordered_set<std::filesystem::path> unmanagedAssets;
	std::unordered_map<std::filesystem::path, std::string> strayMetaData;

	std::unordered_map<kb::UUID, AssetRegistryEntry> assets;
};

static AssetManagerData s_AssetManagerData;

std::unordered_map<kb::UUID, AssetRegistryEntry>& AssetManager::GetManagedAssets() {
	return s_AssetManagerData.assets;
}
std::unordered_set<std::filesystem::path>& AssetManager::GetUnmanagedAssets() {
	return s_AssetManagerData.unmanagedAssets;
}

bool AssetManager::AssetManaged(kb::UUID id) {
	return s_AssetManagerData.assets.contains(id);
}

std::optional<std::filesystem::path> AssetManager::IdToPath(kb::UUID id) {
	if (s_AssetManagerData.idToPath.contains(id) == false)
		return {};
	return s_AssetManagerData.idToPath[id];
}
std::optional<kb::UUID> AssetManager::PathToId(const std::filesystem::path& path) {
	if (s_AssetManagerData.pathToId.contains(path) == false)
		return {};
	return s_AssetManagerData.pathToId[path];
}

AssetType AssetManager::GetAssetType(kb::UUID id) {
	return s_AssetManagerData.assets.at(id).commonMetaData.assetType;
}

void AssetManager::Clear() {
	s_AssetManagerData.unmanagedAssets.clear();
}

void AssetManager::InsertIdPath(const kb::UUID id, const std::filesystem::path& path) {
	if (s_AssetManagerData.idToPath.contains(id))
		throw std::runtime_error("[AssetManager::InsertIdPath] ID already exists");
	if (s_AssetManagerData.pathToId.contains(path))
		throw std::runtime_error("[AssetManager::InsertIdPath] Oath already exists");

	s_AssetManagerData.idToPath[id] = path;
	s_AssetManagerData.pathToId[path] = id;
}

void AssetManager::InsertAssetRegistryEntry(const kb::UUID id, const AssetRegistryEntry& entry) {
	if (s_AssetManagerData.assets.contains(id))
		throw std::runtime_error("[AssetManager::InsertAssetRegistryEntry] ID already exists");

	s_AssetManagerData.assets[id] = entry;
}


void AssetManager::SaveAsset(kb::UUID id) {

	if (s_AssetManagerData.assets.contains(id) == false) {
		spdlog::error("[SaveAsset] Asset: {} is not part of managed assets", id);
		return;
	}

	auto& assetData = s_AssetManagerData.assets[id];
	if (assetData.asset == nullptr) {
		spdlog::error("[SaveAsset] Asset: {} is not loaded", id);
		return;
	}

	const auto assetPath = s_AssetManagerData.idToPath.at(id);

	assetData.asset->SaveAsset(assetPath);
	assetData.commonMetaData.lastModified = std::filesystem::last_write_time(assetPath);

	auto metaPath = assetPath;
	metaPath += ".meta";

	std::ofstream output{ metaPath };
	cereal::JSONOutputArchive archive{ output };

	archive(cereal::make_nvp("meta", assetData.commonMetaData));

	assetData.asset->SaveMeta(archive);
}
void AssetManager::ReloadAsset(kb::UUID id) {
	if (s_AssetManagerData.assets.contains(id) == false) {
		spdlog::error("[ReloadAsset] Asset: {} is not part of managed assets", id);
		return;
	}

	auto& assetData = s_AssetManagerData.assets[id];
	if (assetData.asset == nullptr) {
		spdlog::error("[ReloadAsset] Asset: {} is not loaded", id);
		return;
	}

	const auto assetPath = s_AssetManagerData.idToPath.at(id);

	try {
		assetData.asset->LoadAsset(assetPath);
	}
	catch (std::exception& e) {
		spdlog::error("[LoadAsset] {}", e.what());
	}

	auto metaPath = assetPath;
	metaPath += ".meta";

	std::ifstream input{ metaPath };
	cereal::JSONInputArchive archive{ input };

	archive(cereal::make_nvp("meta", assetData.commonMetaData));

	assetData.asset->LoadMeta(archive);
	assetData.asset->isDirty = true;
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
				if (s_AssetManagerData.assets.contains(fileId)) {
					s_AssetManagerData.assets.erase(fileId);
				}
				
				s_AssetManagerData.idToPath.erase(fileId);
				s_AssetManagerData.pathToId.erase(path);
			}
			if (s_AssetManagerData.unmanagedAssets.contains(path)) {
				s_AssetManagerData.unmanagedAssets.erase(path);
			}
			
			if (std::filesystem::exists(pathMeta)) {
				{
					std::ifstream input{ pathMeta, std::ios::in | std::ios::binary };
					const auto sz = filesystem::file_size(pathMeta);
					std::string metaFile(sz, '\0');
					input.read(metaFile.data(), sz);
					s_AssetManagerData.strayMetaData[path.filename()] = metaFile;
					spdlog::debug("Saved MetaData of {}", pathMeta);
				}
				
				try {
					filesystem::remove(pathMeta);
					spdlog::debug("Removed .meta {}", pathMeta);
				}
				catch (...) {
					spdlog::debug("Couldn't remove {}", pathMeta);
				}
				
			}
			break;
		}
		case efsw::Actions::Modified: {
			// Update timestamp
			const auto lastModified = std::filesystem::last_write_time(path);
			// Update register timestamp
			if (s_AssetManagerData.pathToId.contains(path)) {
				const auto fileId = s_AssetManagerData.pathToId[path];
				s_AssetManagerData.assets.at(fileId).commonMetaData.lastModified = lastModified;
			}
			// Update .meta timestamp
			if (std::filesystem::exists(pathMeta)) {
				//TODO: composite edits of .meta
				//CommonMetaData metaData = CommonMetaData::ReadMetaFile(pathMeta);
				//metaData.lastModified = lastModified;
				//CommonMetaData::WriteMetaFile(pathMeta, metaData);
				//spdlog::debug("Updated MetaData of {}", pathMeta);
			}

			// Reload assset
			// TODO

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

	if (s_AssetManagerData.pathToId.contains(path)) {
		return;
	}

	auto assetType = ExtensionToAssetType(extensionStr);

	if (assetType == AssetType::UNKNOWN) {
		s_AssetManagerData.unmanagedAssets.insert(path);
		return;
	}

	auto metaPath = path;
	metaPath += std::filesystem::path{ ".meta" };

	CommonMetaData metaData{};
	if (const auto strayMetaData = FetchStrayMetaDataRaw(path); strayMetaData) {
		const auto metaDataStr = strayMetaData.value();
		{
			std::ofstream output{ metaPath, std::ios::out | std::ios::binary };
			output.write(metaDataStr.data(), metaDataStr.size());
		}

		metaData = CommonMetaData::ReadMetaFile(metaPath);

		if (metaData.assetType != assetType)
			spdlog::error("Type mismatch loading stray meta data");
	}
	else if (const auto loadedMetaData = FetchFileCommonMetaData(path); loadedMetaData) {
		metaData = loadedMetaData.value();
		if (metaData.assetType != assetType)
			spdlog::error("Type mismatch loading file meta data");
	}
	else {
		metaData.lastModified = std::filesystem::last_write_time(path);
		metaData.assetType = assetType;

		std::ofstream output{ metaPath };
		cereal::JSONOutputArchive archive{ output };
		archive(cereal::make_nvp("meta", metaData));
	}

	s_AssetManagerData.assets[metaData.assetID] = AssetRegistryEntry{ metaData, {} };

	s_AssetManagerData.idToPath[metaData.assetID] = path;
	s_AssetManagerData.pathToId[path] = metaData.assetID;
}

std::optional<std::string> AssetManager::FetchStrayMetaDataRaw(std::filesystem::path path) {
	const auto filename = path.filename();
	if (not s_AssetManagerData.strayMetaData.contains(filename))
		return {};

	auto metaPath = path; metaPath += ".meta";
	auto lastModified = std::filesystem::last_write_time(path);

	std::string metaData = s_AssetManagerData.strayMetaData.at(filename);

	if (std::filesystem::exists(metaPath)) {
		spdlog::error("Stray .meta data available, but file {} already exists", metaPath);
		return {};
	}

	s_AssetManagerData.strayMetaData.erase(path);
	return metaData;
}

std::optional<CommonMetaData> AssetManager::FetchFileCommonMetaData(std::filesystem::path path) {
	auto metaPath = path;
	metaPath += ".meta";

	if (not std::filesystem::exists(metaPath))
		return {};
	
	CommonMetaData loadedMetaData = CommonMetaData::ReadMetaFile(metaPath);
	if (loadedMetaData.lastModified != std::filesystem::last_write_time(path)) {
		spdlog::error("LastModified timestamp mismatch when loading existing .meta file for {}", path);
	}

	return loadedMetaData;
}