#pragma once

#include <Core.hpp>
#include <Assets/AssetTypes/Texture2DAsset.hpp>

struct UniformFloat {
	float value;
	template <typename T>
	void serialize(T& archive) {
		archive(value);
	}
};
struct UniformFloatVec2 {
	glm::vec2 vec;
	template <typename T>
	void serialize(T& archive) {
		archive(vec);
	}
};
struct UniformFloatVec3 {
	glm::vec3 vec;
	template <typename T>
	void serialize(T& archive) {
		archive(vec);
	}
};
struct UniformFloatVec4 {
	glm::vec4 vec;
	template <typename T>
	void serialize(T& archive) {
		archive(vec);
	}
};
struct UniformSampler2D {
	Shared<Texture2DAsset> textureAsset;
	
	template <typename T>
	void load(T& archive) {
		kb::UUID textureId{};
		if (cereal::make_optional_nvp(archive, "textureId", textureId)) {
			textureAsset = AssetManager::GetAsset<Texture2DAsset>(textureId);
		}
	}
	template <typename T>
	void save(T& archive) const {
		archive(cereal::make_nvp("textureId", textureAsset->assetId));
	}
};

using UniformVariant = std::variant<UniformFloat, UniformFloatVec2, UniformFloatVec3, UniformFloatVec4, UniformSampler2D>;

struct Uniform {
	std::string name;
	UniformVariant uniform;

	template <typename T>
	void serialize(T& archive) {
		archive(name, uniform);
	}
};

inline UniformVariant GlTypeToUniformVariant(GLuint type) {
	switch (type) {
	case GL_FLOAT:		return UniformFloat{};
	case GL_FLOAT_VEC2: return UniformFloatVec2{};
	case GL_FLOAT_VEC3: return UniformFloatVec3{};
	case GL_FLOAT_VEC4: return UniformFloatVec4{};
	case GL_SAMPLER_2D: return UniformSampler2D{};
	}
	throw std::runtime_error(std::format("Unknown UniformType: {}", type));
}


