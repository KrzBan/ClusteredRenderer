#include "AssetManager.hpp"

#include "MaterialAsset.hpp"
#include "TextAsset.hpp"
#include "FileUtils.hpp"

#include <cereal/archives/binary.hpp>

struct AssetManagerData {
	std::unordered_map<std::string, MaterialAsset> materialAsset;
	std::unordered_map<std::string, TextAsset> textAssets;
};

static AssetManagerData s_AssetManagerData;

void AssetManager::Init(const std::string& basePath) {
	LoadAssetsAll(basePath);
}

void AssetManager::LoadAssetsAll(const std::string& basePath) {
	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

	for (const auto& dirEntry : recursive_directory_iterator(basePath)) {
		if (dirEntry.is_directory())
			continue;

		auto path = dirEntry.path();
		auto pathStr = path.string();
		auto extension = path.extension().string();

		if (extension == ".txt") {
			s_AssetManagerData.textAssets[pathStr].text = ReadTextFile(path);
		}
		else {
			std::ifstream f(path, std::ios::in | std::ios::binary);
			cereal::BinaryInputArchive iarchive(f);

			if (extension == ".mat") {
				iarchive(s_AssetManagerData.materialAsset[pathStr]);
			}
		}
	}
}

void AssetManager::SaveAssetsAll() {

	for (const auto& [path, asset] : s_AssetManagerData.textAssets) {
		SaveTextFile(path, asset.text);
	}

	for (const auto& [path, material] : s_AssetManagerData.materialAsset) {
		std::ofstream f(path, std::ios::out | std::ios::binary);
		cereal::BinaryOutputArchive oarchive(f);

		oarchive(material);
	}
}