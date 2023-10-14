#include "Shader.hpp"

// ShaderType
std::string ShaderTypeToString(ShaderType shaderType) {
	switch (shaderType) {
	case ShaderType::VERTEX:
		return "VERTEX";
	case ShaderType::FRAGMENT:
		return "FRAGMENT";
	case ShaderType::GEOMETRY:
		return "GEOMRTRY";
	case ShaderType::TESS_CONTROL:
		return "TESS_CONTROL";
	case ShaderType::TESS_EVALUATION:
		return "TESS_EVALUATION";
	case ShaderType::UNKNOWN:
		return "UNKNOWN";
	}

	throw "ShaderTypeToString, Unknown shader type.";
}

uint32 ShaderTypeToGlShaderType(ShaderType shaderType) {
	switch (shaderType) {
	case ShaderType::VERTEX:
		return GL_VERTEX_SHADER;
	case ShaderType::FRAGMENT:
		return GL_FRAGMENT_SHADER;
	case ShaderType::GEOMETRY:
		return GL_GEOMETRY_SHADER;
	case ShaderType::TESS_EVALUATION:
		return GL_TESS_EVALUATION_SHADER;
	case ShaderType::TESS_CONTROL:
		return GL_TESS_CONTROL_SHADER;
		// case ShaderType::COMPUTE: return GL_COMPUTE_SHADER;
	}

	throw std::invalid_argument("ShaderType not supported");
}
// ShaderType End


Shader::Shader(ShaderCreateInfo info)
	: Shader(info, Shader::NameToShaderType(info.filepath), Shader::ReadShaderFile(info.filepath)) {
}

Shader::Shader(ShaderCreateInfo info, ShaderType shaderType, std::string_view source)
	: m_Info{ info },
	  m_ShaderType{ shaderType },
	  m_Handle{ glCreateShader(ShaderTypeToGlShaderType(shaderType)) },
	  m_Source{ PreprocessShader(source) } 
{	
	auto cStr = m_Source.c_str();

	glShaderSource(m_Handle, 1, &cStr, nullptr);
	glCompileShader(m_Handle);

	int success = 0;
	std::array<char, 8192> logBuffer{};

	glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		glGetShaderInfoLog(m_Handle, static_cast<GLsizei>(logBuffer.size()), nullptr, logBuffer.data());
		spdlog::error("Shader compilation failed: {}", logBuffer.data());
	};
}

Shader::Shader(Shader&& other) noexcept 
	: Shader() 
{
	other.Swap(*this);
	other.m_Handle = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
	other.Swap(*this);
	other.m_Handle = 0;

	return *this;
}

void Shader::Swap(Shader& other) noexcept {
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_ShaderType, other.m_ShaderType);
	std::swap(m_Info, other.m_Info);
	std::swap(m_Source, other.m_Source);
}


Shader::~Shader() noexcept {
	Destroy();
}

void Shader::Destroy() {
	if (m_Handle != 0) {
		glDeleteShader(m_Handle);
		m_Handle = 0;
	}
}

uint32 Shader::GetHandle() const {
	return m_Handle;
}

ShaderType Shader::GetShaderType() const {
	return m_ShaderType;
}

std::string Shader::PreprocessShader(std::string_view input) {
	auto source = std::string{ input };
	auto tesX = m_Info.tesCpCountX;
	auto tesY = m_Info.tesCpCountY;
	const size_t maxPoints = 32;

	{	// Tesselation: Number of Control Points
		auto searchStr = std::string{ "CP_COUNT" };
		auto cpCountPos = source.find(searchStr);
		if (cpCountPos != source.npos) {
			source.replace(cpCountPos, searchStr.length(), std::format("{}", std::min(maxPoints, size_t(tesX * tesY))));
		}
	}

	{	// Tesselation: List of Control Points
		auto searchStr = std::string{ "CP_LIST" };
		auto searchStrPos = source.find(searchStr);
		if (searchStrPos != source.npos) {//vec4 p00 = gl_in[0].gl_Position;
			std::string newStr{};
			for (size_t y = 0; y < tesY; ++y) {
				for (size_t x = 0; x < tesX; ++x) {
					if (x + y * tesX >= maxPoints) break;

					newStr.append(std::format("vec4 p{}{} = gl_in[{}].gl_Position;\n", y, x, x + y * tesX));
				}
			}
			source.replace(searchStrPos, searchStr.length(), newStr);
		}

	}

	{	// Tesselation: Bezier, point calculation
		auto searchStr = std::string{ "CP_CALC" };
		auto searchStrPos = source.find(searchStr);
		if (searchStrPos != source.npos) {
			std::string newStr{};

			newStr.append(std::format("float bu[{}], bv[{}];\n", tesX, tesY));
			newStr.append(std::format("float dbu[{}], dbv[{}];\n", tesX, tesY));
			newStr.append(std::format("bernstein{}(bu, dbu, u);\n", tesX));
			newStr.append(std::format("bernstein{}(bv, dbv, v);\n", tesY));

			newStr.append("vec4 pos = \n");
			for (size_t y = 0; y < tesY; ++y) {
				for (size_t x = 0; x < tesX; ++x) {
					if (x + y * tesX >= maxPoints) break;

					if (x != 0 || y != 0) newStr.append(" + ");

					newStr.append(std::format("p{}{}*bu[{}]*bv[{}]", y, x, x, y));
				}
			}
			newStr.append(";\n");

			newStr.append("vec4 du = \n");
			for (size_t x = 0; x < tesX; ++x) {
				for (size_t y = 0; y < tesY; ++y) {
					if (x + y * tesX >= maxPoints) break;

					if (x != 0 || y != 0) newStr.append(" + ");

					newStr.append(std::format("p{}{}*dbu[{}]*bv[{}]", y, x, x, y));
				}
			}
			newStr.append(";\n");

			newStr.append("vec4 dv = \n");
			for (size_t y = 0; y < tesY; ++y) {
				for (size_t x = 0; x < tesX; ++x) {
					if (x + y * tesX >= maxPoints) break;

					if (x != 0 || y != 0) newStr.append(" + ");

					newStr.append(std::format("p{}{}*bu[{}]*dbv[{}]", y, x, x, y));
				}
			}
			newStr.append(";\n");

			source.replace(searchStrPos, searchStr.length(), newStr);
		}
	}

	return source;
}

std::string Shader::ReadShaderFile(std::string_view filepath) {

	std::ifstream fileStream{};
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fileStream.open(std::string(filepath));

	std::stringstream stringStream;
	stringStream << fileStream.rdbuf();

	std::string textSource;
	textSource = stringStream.str();

	// Resolve #includes
	auto pos = textSource.find("#include ");
	while (pos != textSource.npos) {
		const auto p1 = textSource.find('"', pos);
		const auto p2 = textSource.find('"', p1 + 1);
		if (p1 == textSource.npos ||
			p2 == textSource.npos || p2 <= p1) {
			throw std::runtime_error(std::format("Error parsing shader includes: {}", textSource));
		}

		const auto fileDir = filepath.substr(0, filepath.rfind('/'));
		const std::string name = textSource.substr(p1 + 1, p2 - p1 - 1);
		const std::string include = ReadShaderFile(std::format("{}/{}", fileDir, name));
		textSource.replace(pos, p2 - pos + 1, include);

		pos = textSource.find("#include ");
	}

	return textSource;
}

ShaderType Shader::NameToShaderType(std::string_view filename) {
	if (filename.ends_with(".vert")) return ShaderType::VERTEX;
	else if (filename.ends_with(".frag")) return ShaderType::FRAGMENT;
	else if (filename.ends_with(".geom")) return ShaderType::GEOMETRY;
	else if (filename.ends_with(".tese")) return ShaderType::TESS_EVALUATION;
	else if (filename.ends_with(".tesc")) return ShaderType::TESS_CONTROL;

	throw std::invalid_argument("Shader extension not supported");
}