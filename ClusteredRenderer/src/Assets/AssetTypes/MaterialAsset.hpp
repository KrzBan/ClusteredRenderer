#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

#include "ShaderAsset.hpp"

#include <Renderer/UniformTypes.hpp>

struct MaterialAsset : public Asset {
	MaterialAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MATERIAL; };

	virtual void LoadAsset(const std::filesystem::path& path) override;
	virtual void SaveAsset(const std::filesystem::path& path) const override;

	Shared<ShaderAsset> shaderAsset;
	std::vector<Uniform> uniforms;
};

template <>
inline AssetType AssetTypeFromType<MaterialAsset>() {
	return AssetType::MATERIAL;
}