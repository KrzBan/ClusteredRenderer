#pragma once

#include <Core.hpp>
#include "Shader.hpp"


using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

UniformValue UniformValueDefaultFromType(GLenum type);

struct UniformInfo {
	std::string name;
	GLenum type;
	UniformValue value;
};

class ShaderProgram {
public:
	ShaderProgram();

	~ShaderProgram() noexcept;

	void AttachShader(Shader& shader);
	void CompileProgram() const;

	void Bind() const;
	void Unbind() const;
	uint32 GetHandle() const;

	std::vector<UniformInfo> GetAlluniformInfo();

	void SetBool(std::string_view name, bool value) const;
	void SetInt(std::string_view name, int value) const;
	void SetIntArray(std::string_view name, int* values, uint32_t count) const;
	void SetFloat(std::string_view name, float value) const;
	void SetFloat2(std::string_view name, const glm::vec2&) const;
	void SetFloat3(std::string_view name, const glm::vec3&) const;
	void SetFloat4(std::string_view name, const glm::vec4&) const;
	void SetMat4(std::string_view name, const glm::mat4& mat)const;

private:
	uint32_t m_Handle = 0;

	std::unordered_map<ShaderType, Shader> m_Shaders;
};