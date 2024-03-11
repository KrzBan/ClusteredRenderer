#pragma once

#include <Core.hpp>

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
	int id;
	template <typename T>
	void serialize(T& archive) {
		archive(id);
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


