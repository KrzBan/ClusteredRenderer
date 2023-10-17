#pragma once

#include <Core.hpp>

struct MaterialAsset {
	int test = 0;

	template <class Archive>
	void serialize(Archive& archive) {
		archive(test);
	}
};