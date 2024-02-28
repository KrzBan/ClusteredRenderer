#pragma once

#include <Core.hpp>
#include <UUID.hpp>

#include "Assets.hpp"
#include "AssetWatcher.hpp"
#include "CommonMetaData.hpp"

struct AssetRegistryEntry {
	CommonMetaData commonMetaData;
	Shared<Unique<Asset>> asset;
};

struct AssetManager {
public:
	static void Init(const std::string& basePath);

	static void HandleFileChanges(const std::vector<FileActions>& queue);

	static const std::unordered_map<kb::UUID, AssetRegistryEntry>& GetManagedAssets();
	static const std::unordered_set<std::filesystem::path>& GetUnmanagedAssets();

	static std::filesystem::path GetPath(kb::UUID);

	static void Clear();

private:
	static void DiscoverAssets(const std::string& basePath);

	static void AddFile(const std::filesystem::path& path);

	static std::optional<std::string> FetchStrayMetaDataRaw(std::filesystem::path path);
	static std::optional<CommonMetaData> FetchFileCommonMetaData(std::filesystem::path path);
};