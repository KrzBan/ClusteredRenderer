#pragma once

#include <Core.hpp>

enum class AssetType {
	UNKNOWN,
	MATERIAL,
	TEXT,
	TEXTURE_2D
};

constexpr std::string AssetTypeToString(AssetType type) {
	switch (type) {
	case AssetType::UNKNOWN:
		return "Unknown";
	case AssetType::MATERIAL:
		return "Material";
	case AssetType::TEXT:
		return "Text";
	case AssetType::TEXTURE_2D:
		return "Texture 2D";
	}

	throw std::invalid_argument("Invalid enum value");
}

constexpr AssetType ExtensionToAssetType(const std::string& ext) {
	if (ext == ".mat") {
		return AssetType::MATERIAL;
	}
	if (ext == ".txt") {
		return AssetType::TEXT;
	}
	if (ext == ".png") {
		return AssetType::TEXTURE_2D;
	}
	return AssetType::UNKNOWN;
}

class Texture2D;
template <typename T>
constexpr AssetType TypeToAssetType() {
	return AssetType::UNKNOWN;
}
template <>
constexpr AssetType TypeToAssetType<Texture2D>() {
	return AssetType::TEXTURE_2D;
}