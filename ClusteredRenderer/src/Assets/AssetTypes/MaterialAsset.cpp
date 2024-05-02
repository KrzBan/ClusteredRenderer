#include "MaterialAsset.hpp"

#include <Renderer/Renderer.hpp>

void MaterialAsset::LoadAsset(const std::filesystem::path& path) {
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
				std::ranges::find_if(uniforms, [&](const Uniform& other) { return other.name == uniform.name; });
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

void MaterialAsset::SaveAsset(const std::filesystem::path& path) const {
	std::ofstream output(path);
	cereal::JSONOutputArchive archive(output);

	if (shaderAsset)
		archive(cereal::make_nvp("shader_id", shaderAsset->assetId));

	archive(uniforms);
}