#pragma once

#include <Core.hpp>

enum class ShaderType {
	UNKNOWN,
	VERTEX, FRAGMENT,
	GEOMETRY, TESS_CONTROL,
	TESS_EVALUATION
	//, COMPUTE
};

uint32 ShaderTypeToGlShaderType(ShaderType shaderType);
std::string ShaderTypeToString(ShaderType shaderType);

struct ShaderCreateInfo {
	std::string_view filepath;
	uint32_t tesCpCountX = 4;
	uint32_t tesCpCountY = 4;
	ShaderCreateInfo() = default;
	ShaderCreateInfo(std::string_view filepath, uint32_t tesCpCountX = 4, uint32_t tesCpCountY = 4)
		: filepath{ filepath }, tesCpCountX{ tesCpCountX }, tesCpCountY{ tesCpCountY } {}
};

class Shader {
public:
	Shader() = default;
	Shader(ShaderCreateInfo info);
	Shader(ShaderCreateInfo info, ShaderType shaderType, std::string_view source);
	Shader(Shader&& other) noexcept;
	~Shader() noexcept;

	Shader& operator=(Shader&& other) noexcept;

	void Swap(Shader& other) noexcept;

	uint32 GetHandle() const;
	ShaderType GetShaderType() const;

private:
	uint32 m_Handle = 0;
	ShaderType m_ShaderType = ShaderType::UNKNOWN;
	ShaderCreateInfo m_Info;
	std::string m_Source;

private:
	void Destroy();

	std::string PreprocessShader(std::string_view input);

	std::string ReadShaderFile(std::string_view filepath);
	ShaderType NameToShaderType(std::string_view filename);
};