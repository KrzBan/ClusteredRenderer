#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct MaterialAsset {
	int test = 0;

	template <class Archive>
	void serialize(Archive& archive) {
		archive(test);
	}
};

template <>
constexpr AssetType TypeToAssetType<MaterialAsset>() {
	return AssetType::MATERIAL;
}