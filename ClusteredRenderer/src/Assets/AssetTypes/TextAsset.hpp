#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct TextAsset {

	TextAsset(const std::filesystem::path& source) {
		std::ifstream input(source, std::ios::in);

		const auto size = std::filesystem::file_size(source);

		text.resize(size);

		input.read(text.data(), size);
	}

	std::string text;
};

template <>
constexpr AssetType TypeToAssetType<TextAsset>() {
	return AssetType::TEXT;
}