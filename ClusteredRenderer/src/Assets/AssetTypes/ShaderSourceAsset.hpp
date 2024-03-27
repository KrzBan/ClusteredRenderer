#pragma once

#include <Core.hpp>
#include <Utils/FileUtils.hpp>

#include "AssetType.hpp"

enum class ShaderSourceType{
	UNKNOWN,
	VERTEX,
	TESSELATION_CONTROL,
	TESSELATION_EVALUTATION,
	GEOMETRY,
	FRAGMENT,
	COMPUTE
};

inline constexpr ShaderSourceType FileExtToShaderSourceType(const std::string& ext) {
	if (ext == ".vert") 
		return ShaderSourceType::VERTEX;
	if (ext == ".tesc")
		return ShaderSourceType::TESSELATION_CONTROL;
	if (ext == ".tese")
		return ShaderSourceType::TESSELATION_EVALUTATION;
	if (ext == ".geom")
		return ShaderSourceType::GEOMETRY;
	if (ext == ".frag")
		return ShaderSourceType::FRAGMENT;
	if (ext == ".comp")
		return ShaderSourceType::COMPUTE;
	return ShaderSourceType::UNKNOWN;
}

struct ShaderSourceAsset : public Asset {
	ShaderSourceAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::SHADER_SOURCE; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		source = LoadFileText(path);

		type = FileExtToShaderSourceType(path.extension().string());
	}
	virtual void SaveAsset(const std::filesystem::path& path) const override {
		std::ofstream output(path);
		output << source;
	}

	std::string source;
	ShaderSourceType type = ShaderSourceType::UNKNOWN;
};

template <>
inline AssetType AssetTypeFromType<ShaderSourceAsset>() {
	return AssetType::SHADER_SOURCE;
}