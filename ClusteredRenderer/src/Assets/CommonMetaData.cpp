#include "CommonMetaData.hpp"

CommonMetaData CommonMetaData::ReadMetaFile(const std::filesystem::path& path) {
	CommonMetaData meta;
	std::ifstream f(path, std::ios::in);
	cereal::JSONInputArchive iarchive(f);

	iarchive(cereal::make_nvp("meta", meta));

	return meta;
}

void CommonMetaData::WriteMetaFile(const std::filesystem::path& path, const CommonMetaData& metaData) {
	std::ofstream f(path);
	cereal::JSONOutputArchive oarchive(f);

	WriteMetaFile(oarchive, metaData);
}

void CommonMetaData::WriteMetaFile(cereal::JSONOutputArchive& oarchive, const CommonMetaData& metaData) {
	oarchive(cereal::make_nvp("meta", metaData));
}