#include "ShaderProgram.hpp"

UniformValue UniformValueDefaultFromType(GLenum type) {
	switch (type) {
	case GL_BOOL:
		return bool();
	case GL_INT:
		return int();
	case GL_FLOAT:
		return float();
	case GL_FLOAT_VEC2:
		return glm::vec2();
	case GL_FLOAT_VEC3:
		return glm::vec3();
	case GL_FLOAT_VEC4:
		return glm::vec4();
	case GL_FLOAT_MAT4:
		return glm::mat4();
	}

	throw std::format("UniformValueDefaultFromType Unknown GLenum type: ", type);
}

ShaderProgram::ShaderProgram()
	:m_Handle(glCreateProgram())
{}

ShaderProgram::~ShaderProgram() noexcept {
	glDeleteProgram(m_Handle);
}

void ShaderProgram::AttachShader(Shader& shader) {
	auto shaderType = shader.GetShaderType();
	if (m_Shaders.contains(shaderType)) {
		spdlog::error("ShaderProgram already attached shader of type: {}", ShaderTypeToString(shaderType));
		return;
	}

	glAttachShader(m_Handle, shader.GetHandle());
	m_Shaders[shaderType] = std::move(shader);
}

void ShaderProgram::CompileProgram() const {
	glLinkProgram(m_Handle);

	int success = 0;
	std::array<char, 8192> logBuffer{};

	glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		glGetProgramInfoLog(m_Handle, static_cast<GLsizei>(logBuffer.size()), nullptr, logBuffer.data());
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

std::vector<UniformInfo> ShaderProgram::GetAlluniformInfo() {
	constexpr int maxBufSize = 8192;
	char nameBuf[maxBufSize] = {};
	int bufLength = 0;

	GLint size = 0;
	GLenum type = 0;

	int count = 0;
	glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &count);

	std::vector<UniformInfo> uniforms;
	for (size_t i = 0; i < count; i++) {
		glGetActiveUniform(m_Handle, (GLuint)i, maxBufSize, &bufLength, &size, &type, nameBuf);

		uniforms.push_back({ nameBuf, type, UniformValueDefaultFromType(type) });
	}

	return uniforms;
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
void ShaderProgram::SetMat4(std::string_view name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_Handle, std::string{ name }.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}