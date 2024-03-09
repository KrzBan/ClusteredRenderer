#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

#include "ShaderSourceAsset.hpp"
#include <Assets/AssetManager.hpp>

struct ShaderAsset : public Asset {
	ShaderAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::SHADER; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		std::ifstream input(path);
		cereal::JSONInputArchive archive(input);
		
		kb::UUID vertex_id{};
		if (cereal::make_optional_nvp(archive, "vertex_id", vertex_id)) {
			vertex = AssetManager::GetAsset<ShaderSourceAsset>(vertex_id);
		}
		kb::UUID tesselation_control_id{};
		if (cereal::make_optional_nvp(archive, "tesselation_control_id", tesselation_control_id)) {
			tesselation_control = AssetManager::GetAsset<ShaderSourceAsset>(tesselation_control_id);
		}
		kb::UUID tesselation_evaluation_id{};
		if (cereal::make_optional_nvp(archive, "tesselation_evaluation_id", tesselation_evaluation_id)) {
			tesselation_evaluation = AssetManager::GetAsset<ShaderSourceAsset>(tesselation_evaluation_id);
		}
		kb::UUID geometry_id{};
		if (cereal::make_optional_nvp(archive, "geometry_id", geometry_id)) {
			geometry = AssetManager::GetAsset<ShaderSourceAsset>(geometry_id);
		}
		kb::UUID fragment_id{};
		if (cereal::make_optional_nvp(archive, "fragment_id", fragment_id)) {
			fragment = AssetManager::GetAsset<ShaderSourceAsset>(fragment_id);
		}
		kb::UUID compute_id{};
		if (cereal::make_optional_nvp(archive, "compute_id", compute_id)) {
			compute = AssetManager::GetAsset<ShaderSourceAsset>(compute_id);
		}
	}
	virtual void SaveAsset(const std::filesystem::path& path) const override {
		std::ofstream output(path);
		cereal::JSONOutputArchive archive(output);

		if (vertex) archive(cereal::make_nvp("vertex_id", vertex->assetId));
		if (tesselation_control) archive(cereal::make_nvp("tesselation_control_id", tesselation_control->assetId));
		if (tesselation_evaluation) archive(cereal::make_nvp("tesselation_evaluation_id", tesselation_evaluation->assetId));
		if (geometry) archive(cereal::make_nvp("geometry_id", geometry->assetId));
		if (fragment) archive(cereal::make_nvp("fragment_id", fragment->assetId));
		if (compute) archive(cereal::make_nvp("compute_id", compute->assetId));
	}	

	Shared<ShaderSourceAsset> vertex;
	Shared<ShaderSourceAsset> tesselation_control;
	Shared<ShaderSourceAsset> tesselation_evaluation;
	Shared<ShaderSourceAsset> geometry;
	Shared<ShaderSourceAsset> fragment;
	Shared<ShaderSourceAsset> compute;

	std::string status = "Not built";
};

template <>
inline AssetType AssetTypeFromType<ShaderAsset>() {
	return AssetType::SHADER;
}