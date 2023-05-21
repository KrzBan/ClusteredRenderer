#pragma once

#include <Core.hpp>
#include "Shader.hpp"

class ShaderProgram {
public:
	ShaderProgram();

	~ShaderProgram() noexcept;

	void AttachShader(const Shader& shader) const;
	void CompileProgram() const;

	void Bind() const;
	void Unbind() const;
	uint32 GetHandle() const;

	void SetBool(std::string_view name, bool value) const;
	void SetInt(std::string_view name, int value) const;
	void SetIntArray(std::string_view name, int* values, uint32_t count) const;
	void SetFloat(std::string_view name, float value) const;
	void SetFloat2(std::string_view name, const glm::vec2&) const;
	void SetFloat3(std::string_view name, const glm::vec3&) const;
	void SetFloat4(std::string_view name, const glm::vec4&) const;
	void SetMat4f(std::string_view name, const glm::mat4& mat)const;

private:
	uint32_t m_Handle = 0;
};