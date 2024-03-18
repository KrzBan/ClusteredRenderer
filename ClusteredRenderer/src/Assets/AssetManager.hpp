#pragma once

#include <Core.hpp>
#include <UUID.hpp>

#include "AssetTypes/AssetType.hpp"
#include "AssetWatcher.hpp"
#include "CommonMetaData.hpp"

struct AssetRegistryEntry {
	CommonMetaData commonMetaData;
	Shared<Asset> asset;
};

struct AssetManager {
public:
	static void Init(const std::string& basePath);

	static void HandleFileChanges(const std::vector<FileActions>& queue);

	static std::unordered_map<kb::UUID, AssetRegistryEntry>& GetManagedAssets();
	static std::unordered_set<std::filesystem::path>& GetUnmanagedAssets();

	static void Clear();
	static bool AssetManaged(kb::UUID id);

	template <typename T>
	static std::shared_ptr<T> GetAsset(kb::UUID id);
	static void SaveAsset(kb::UUID id);
	static void ReloadAsset(kb::UUID id);

	template<typename T>
	static void CreateAsset(const std::filesystem::path& path, const std::string& name);

	static AssetType GetAssetType(kb::UUID id);

	static std::optional<std::filesystem::path> IdToPath(kb::UUID id);
	static std::optional<kb::UUID> PathToId(const std::filesystem::path& path);

private:
	static void DiscoverAssets(const std::string& basePath);

	static void AddFile(const std::filesystem::path& path);
	static void InsertIdPath(const kb::UUID id, const std::filesystem::path& path);
	static void InsertAssetRegistryEntry(const kb::UUID id, const AssetRegistryEntry& entry);

	static std::optional<std::string> FetchStrayMetaDataRaw(std::filesystem::path path);
	static std::optional<CommonMetaData> FetchFileCommonMetaData(std::filesystem::path path);
};

template <typename T>
std::shared_ptr<T> AssetManager::GetAsset(kb::UUID id) {
	if (id == 0)
		return nullptr;

	if (AssetManaged(id) == false) {
		spdlog::error("Asset ID: {} unmanaged", id);
		return nullptr;
	}

	auto& assetEntry = GetManagedAssets().at(id);
	if (assetEntry.commonMetaData.assetType != AssetTypeFromType<T>()) {
		spdlog::error("Asset ID: {} type mismatch", id);
		return nullptr;
	}

	if (assetEntry.asset != nullptr) {
		const auto cast = std::dynamic_pointer_cast<T>(assetEntry.asset);
		if (cast == nullptr) {
			throw std::runtime_error("GetAsset() dynamic_pointer_cast failed");
		}

		return cast;
	}

	// Create new
	auto sharedAsset = std::make_shared<T>();
	sharedAsset->assetId = id;

	const auto assetPath = IdToPath(id).value();
	auto metaPath = assetPath;
	metaPath += std::filesystem::path(".meta");

	std::ifstream input(metaPath);
	cereal::JSONInputArchive archive(input);
	CommonMetaData cmd{};
	archive(cmd);

	(*sharedAsset).LoadMeta(archive);
	try {
		(*sharedAsset).LoadAsset(assetPath);
	}
	catch (std::exception& e) {
		spdlog::error("[LoadAsset] {}", e.what());
	}

	Shared<Asset> sharedBase = std::static_pointer_cast<Asset>(sharedAsset);
	assetEntry.asset = sharedBase;

	return sharedAsset;
}

template <typename T>
void AssetManager::CreateAsset(const std::filesystem::path& path, const std::string& name) {

	auto filePath = path / name;

	uint32 nameId = 1;
	while (std::filesystem::exists(filePath)) {
		const auto nameBase = name.substr(0, name.find_last_of('.'));
		const auto extension = name.substr(name.find_last_of('.'));
		const auto newName = std::format("{} ({}){}", nameBase, nameId++, extension);
		filePath = path / newName;
	}

	kb::UUID assetId{};
	InsertIdPath(assetId, filePath);

	auto sharedAsset = std::make_shared<T>();
	sharedAsset->assetId = assetId;

	sharedAsset->SaveAsset(filePath);
	const auto lastModified = std::filesystem::last_write_time(filePath);

	CommonMetaData metaData{};
	metaData.assetID = assetId;
	metaData.assetType = AssetTypeFromType<T>();
	metaData.lastModified = lastModified;

	AssetRegistryEntry registryEntry{};
	registryEntry.asset = sharedAsset;
	registryEntry.commonMetaData = metaData;

	InsertAssetRegistryEntry(assetId, registryEntry);

	auto metaPath = filePath;
	metaPath += std::filesystem::path(".meta");

	std::ofstream output{ metaPath };
	cereal::JSONOutputArchive archive{ output };

	archive(cereal::make_nvp("meta", metaData));
	sharedAsset->SaveMeta(archive);
}