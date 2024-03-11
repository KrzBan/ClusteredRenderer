#pragma once

#include <Core.hpp>

struct UniformSampler2D {
	int id;

	template<typename T>
	void serialize(T& archive) {
		archive(id);
	}
};
struct UniformVec3 {
	glm::vec3 vec;
	template <typename T>
	void serialize(T& archive) {
		archive(vec);
	}
};

using UniformVariant = std::variant<UniformSampler2D, UniformVec3>;

struct Uniform {
	std::string name;
	UniformVariant uniform;

	template <typename T>
	void serialize(T& archive) {
		archive(name, uniform);
	}
};

inline UniformVariant GlTypeToUniformVariant(GLuint type) {
	return UniformVec3{};
}
