#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct UniformSampler2D {
	std::string name;
	int id;

	template<typename T>
	void serialize(T& archive) {
		archive(name, id);
	}
};
struct UniformVec3 {
	std::string name;
	glm::vec3 vec;
	template <typename T>
	void serialize(T& archive) {
		archive(name, vec);
	}
};
using UniformType = std::variant<UniformSampler2D, UniformVec3>;

struct MaterialAsset : public Asset {
	MaterialAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::MATERIAL; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		std::ifstream input(path);
		cereal::JSONInputArchive archive(input);

		archive(uniforms);

	}
	virtual void SaveAsset(const std::filesystem::path& path) const override {
		std::ofstream output(path);
		cereal::JSONOutputArchive archive(output);

		archive(uniforms);
	}

	std::vector<UniformType> uniforms;
};

template <>
inline AssetType AssetTypeFromType<MaterialAsset>() {
	return AssetType::MATERIAL;
}