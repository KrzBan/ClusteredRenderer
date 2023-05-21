#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram()
	:m_Handle(glCreateProgram())
{}

ShaderProgram::~ShaderProgram() noexcept {
	glDeleteProgram(m_Handle);
}

void ShaderProgram::AttachShader(const Shader& shader) const {
	glAttachShader(m_Handle, shader.GetHandle());
}
void ShaderProgram::CompileProgram() const {
	glLinkProgram(m_Handle);

	int success = 0;
	std::array<char, 8192> logBuffer{};

	glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		glGetProgramInfoLog(m_Handle, logBuffer.size(), nullptr, logBuffer.data());
		spdlog::error("ShaderProgram link failed: {}", logBuffer.data());
	}
}

void ShaderProgram::Bind() const {
	glUseProgram(m_Handle);
}
void ShaderProgram::Unbind() const {
	glUseProgram(0);
}

uint32_t ShaderProgram::GetHandle() const {
	return m_Handle;
}

void ShaderProgram::SetBool(std::string_view name, bool value) const {
	glUniform1i(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), (int)value);
}
void ShaderProgram::SetInt(std::string_view name, int value) const {
	glUniform1i(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), value);
}
void ShaderProgram::SetIntArray(std::string_view name, int* values, uint32_t count) const {
	glUniform1iv(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), count, values);
}
void ShaderProgram::SetFloat(std::string_view name, float value) const {
	glUniform1f(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), value);
}
void ShaderProgram::SetFloat2(std::string_view name, const glm::vec2& vec) const {
	glUniform2f(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), vec.x, vec.y);
}
void ShaderProgram::SetFloat3(std::string_view name, const glm::vec3& vec) const {
	glUniform3f(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), vec.x, vec.y, vec.z);
}
void ShaderProgram::SetFloat4(std::string_view name, const glm::vec4& vec) const {
	glUniform4f(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), vec.x, vec.y, vec.z, vec.w);
}
void ShaderProgram::SetMat4f(std::string_view name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}