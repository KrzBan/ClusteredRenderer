#pragma once

#include <Core.hpp>

struct AssetManager {

	static void Init(const std::string& basePath);
	static void LoadAssetsAll(const std::string& basePath);
	static void SaveAssetsAll();
};