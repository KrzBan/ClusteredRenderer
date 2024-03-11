#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

#include "ShaderAsset.hpp"

#include <Renderer/Renderer.hpp>
#include <Renderer/UniformTypes.hpp>

struct MaterialAsset : public Asset {
	MaterialAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MATERIAL; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		std::ifstream input(path);
		cereal::JSONInputArchive archive(input);

		kb::UUID shader_id{};
		if (cereal::make_optional_nvp(archive, "shader_id", shader_id)) {
			shaderAsset = AssetManager::GetAsset<ShaderAsset>(shader_id);
		}

		if (shaderAsset != nullptr) {
			uniforms = Renderer::QueryShaderUniforms(*shaderAsset);

			std::vector<Uniform> loadedUniforms;
			archive(loadedUniforms);

			for (const auto& uniform : loadedUniforms) {
				auto find = 
					std::ranges::find_if(uniforms, [](const Uniform& other) { return other.name == " lmao "; });
				if (find == uniforms.end())
					continue;

				auto& foundUniform = *find;
				if (foundUniform.uniform.index() != uniform.uniform.index()) {
					spdlog::error("[MaterialAsset::LoadAsset] Uniform type mismatch!");
					continue;
				}

				foundUniform.uniform = uniform.uniform;
			}
		}
	}

	virtual void SaveAsset(const std::filesystem::path& path) const override {
		std::ofstream output(path);
		cereal::JSONOutputArchive archive(output);

		if (shaderAsset) archive(cereal::make_nvp("shader_id", shaderAsset->assetId));

		archive(uniforms);
	}

	Shared<ShaderAsset> shaderAsset;
	std::vector<Uniform> uniforms;
};

template <>
inline AssetType AssetTypeFromType<MaterialAsset>() {
	return AssetType::MATERIAL;
}