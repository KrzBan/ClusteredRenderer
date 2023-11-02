#include "MetaData.hpp"

MetaData MetaData::ReadMetaFile(const std::filesystem::path& path) {
	MetaData meta;
	std::ifstream f(path, std::ios::in);
	cereal::JSONInputArchive iarchive(f);

	iarchive(cereal::make_nvp("meta", meta));

	return meta;
}
void MetaData::WriteMetaFile(const std::filesystem::path& path, const MetaData& metaData) {
	std::ofstream f(path, std::ios::out);
	cereal::JSONOutputArchive oarchive(f);

	oarchive(cereal::make_nvp("meta", metaData));
}