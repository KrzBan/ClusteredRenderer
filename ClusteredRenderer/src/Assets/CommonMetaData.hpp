#pragma once

#include <Core.hpp>
#include <UUID.hpp>
#include "AssetTypes/AssetType.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/archives/json.hpp>

constexpr uint32 g_MetaFileVersion = 1;

struct CommonMetaData {
	kb::UUID assetID;
	std::filesystem::file_time_type lastModified;
	AssetType assetType;

	static CommonMetaData ReadMetaFile(const std::filesystem::path& path);
	static void WriteMetaFile(cereal::JSONOutputArchive& oarchive, const CommonMetaData& metaData);

	template <class Archive>
	void save(Archive& archive) const {
		archive(
			cereal::make_nvp("version", g_MetaFileVersion), 
			cereal::make_nvp("file_id", assetID), 
			cereal::make_nvp("last_modified", lastModified),
			cereal::make_nvp("asset_type", std::string(magic_enum::enum_name(assetType))));
	}

	template <class Archive>
	void load(Archive& archive) {
		uint32 metaFileVersion{};
		archive(
			cereal::make_nvp("version", metaFileVersion));
		if (metaFileVersion != g_MetaFileVersion) {
			throw std::runtime_error(
				std::format("MeteFileVersion mismatch. Read {}, but current is {}", metaFileVersion, g_MetaFileVersion));
		}

		std::string assetTypeStr;

		archive(
			cereal::make_nvp("file_id", assetID),
			cereal::make_nvp("last_modified", lastModified),
			cereal::make_nvp("asset_type", assetTypeStr));

		assetType = magic_enum::enum_cast<AssetType>(assetTypeStr).value_or(AssetType::UNKNOWN);
	}
};