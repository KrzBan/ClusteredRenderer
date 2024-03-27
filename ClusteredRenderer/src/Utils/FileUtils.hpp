#pragma once

#include <Core.hpp>

inline std::string LoadFileText(std::filesystem::path path) {
	std::ifstream input(path);
	const auto size = std::filesystem::file_size(path);

	std::string text{};
	text.resize(size);
	input.read(text.data(), size);

	return text;
}