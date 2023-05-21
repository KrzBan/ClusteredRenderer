#pragma once

#include <Core.hpp>

enum class ShaderType {
	VERTEX, FRAGMENT,
	GEOMETRY, TESS_CONTROL,
	TESS_EVALUATION
	//, COMPUTE
};

struct ShaderCreateInfo {
	std::string_view filepath = "";
	uint32_t tesCpCountX = 4;
	uint32_t tesCpCountY = 4;
	ShaderCreateInfo() = default;
	ShaderCreateInfo(std::string_view filepath, uint32_t tesCpCountX = 4, uint32_t tesCpCountY = 4)
		: filepath{ filepath }, tesCpCountX{ tesCpCountX }, tesCpCountY{ tesCpCountY } {}
};

class Shader {
public:
	Shader(ShaderCreateInfo info);
	Shader(ShaderType shaderType, std::string_view source, ShaderCreateInfo info);
	~Shader() noexcept;
	uint32 GetHandle() const;

private:
	uint32 m_Handle = 0;
	ShaderType m_ShaderType;
	ShaderCreateInfo m_Info;

private:
	std::string PreprocessShader(const std::string& input);

	std::string ReadShaderFile(std::string_view filepath);
	ShaderType NameToShaderType(std::string_view filename);
	uint32 ShaderTypeToShaderType(ShaderType shaderType);
};