#pragma once

#include <Core.hpp>

std::string ReadTextFile(std::filesystem::path path) {
	std::ifstream f(path, std::ios::in | std::ios::binary);
	const auto sz = std::filesystem::file_size(path);

	std::string result(sz, '\0');
	f.read(result.data(), sz);

	return result;
}

void SaveTextFile(std::filesystem::path path, const std::string& text) {
	std::ofstream f(path, std::ios::out | std::ios::binary);
	const auto sz = text.size();

	f.write(text.data(), sz);
}