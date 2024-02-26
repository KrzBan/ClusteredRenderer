#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct MaterialAsset : public Asset {
	MaterialAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MATERIAL; };
};

template <>
inline AssetType AssetTypeFromType<MaterialAsset>() {
	return AssetType::MATERIAL;
}