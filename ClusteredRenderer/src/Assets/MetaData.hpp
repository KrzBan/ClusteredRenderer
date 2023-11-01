#pragma once

#include <Core.hpp>
#include <UUID.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/archives/json.hpp>

constexpr uint32 g_MetaFileVersion = 1;

struct MetaData {
	kb::UUID assetID;
	std::filesystem::file_time_type lastModified;

	template <class Archive>
	void save(Archive& archive) const {
		archive(
			cereal::make_nvp("version", g_MetaFileVersion), 
			cereal::make_nvp("file_id", assetID), 
			cereal::make_nvp("last_modified", lastModified));
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

		archive(
			cereal::make_nvp("file_id", assetID),
			cereal::make_nvp("last_modified", lastModified));
	}
};