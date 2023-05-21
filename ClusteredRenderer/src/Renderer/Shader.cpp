#include "Shader.hpp"


Shader::Shader(ShaderCreateInfo info)
	: Shader(Shader::NameToShaderType(info.filepath), Shader::ReadShaderFile(info.filepath), info)
{
}

Shader::Shader(ShaderType shaderType, std::string_view source, ShaderCreateInfo info)
	: m_ShaderType{ shaderType },
	m_Handle{ glCreateShader(ShaderTypeToShaderType(shaderType)) },
	m_Info{ info }
{
	auto str = std::string{ source };
	str = PreprocessShader(str);
	auto cStr = str.c_str();

	glShaderSource(m_Handle, 1, &cStr, nullptr);
	glCompileShader(m_Handle);

	int success = 0;
	std::array<char, 8192> logBuffer{};

	glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		glGetShaderInfoLog(m_Handle, logBuffer.size(), nullptr, logBuffer.data());
		spdlog::error("Shader compilation failed: {}", logBuffer.data());
	};
}

Shader::~Shader() noexcept {
	glDeleteShader(m_Handle);
}

uint32_t Shader::GetHandle() const {
	return m_Handle;
}

uint32_t Shader::ShaderTypeToShaderType(ShaderType shaderType) {
	switch (shaderType) {
	case ShaderType::VERTEX: return GL_VERTEX_SHADER;
	case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
	case ShaderType::GEOMETRY: return GL_GEOMETRY_SHADER;
	case ShaderType::TESS_EVALUATION: return GL_TESS_EVALUATION_SHADER;
	case ShaderType::TESS_CONTROL: return GL_TESS_CONTROL_SHADER;
		//case ShaderType::COMPUTE: return GL_COMPUTE_SHADER;
	}

	throw std::invalid_argument("ShaderType not supported");
}

std::string Shader::PreprocessShader(const std::string& input) {
	auto source = input;
	auto tesX = m_Info.tesCpCountX;
	auto tesY = m_Info.tesCpCountY;
	const size_t maxPoints = 32;
	{
		auto searchStr = std::string{ "CP_COUNT" };
		auto cpCountPos = source.find(searchStr);
		if (cpCountPos != source.npos) {
			source.replace(cpCountPos, searchStr.length(), std::format("{}", std::min(maxPoints, size_t(tesX * tesY))));
		}
	}

	{
		auto searchStr = std::string{ "CP_LIST" };
		auto searchStrPos = source.find(searchStr);
		if (searchStrPos != source.npos) {//vec4 p00 = gl_in[0].gl_Position;
			std::string newStr{};
			for (int y = 0; y < tesY; ++y) {
				for (int x = 0; x < tesX; ++x) {
					if (x + y * tesX >= maxPoints) break;

					newStr.append(std::format("vec4 p{}{} = gl_in[{}].gl_Position;\n", y, x, x + y * tesX));
				}
			}
			source.replace(searchStrPos, searchStr.length(), newStr);
		}

	}

	{
		auto searchStr = std::string{ "CP_CALC" };
		auto searchStrPos = source.find(searchStr);
		if (searchStrPos != source.npos) {
			std::string newStr{};

			//float bu[4], bv[4]; // Basis functions for u and v
			//float dbu[4], dbv[4]; // Derivitives for u and v
			//bernstein4(bu, dbu, u);
			//bernstein4(bv, dbv, v);

			newStr.append(std::format("float bu[{}], bv[{}];\n", tesX, tesY));
			newStr.append(std::format("float dbu[{}], dbv[{}];\n", tesX, tesY));
			newStr.append(std::format("bernstein{}(bu, dbu, u);\n", tesX));
			newStr.append(std::format("bernstein{}(bv, dbv, v);\n", tesY));

			//vec4 pos =
			//p00*bu[0]*bv[0] + p01*bu[0]*bv[1] + p02*bu[0]*bv[2] + p03*bu[0]*bv[3] +
			//p10*bu[1]*bv[0] + p11*bu[1]*bv[1] + p12*bu[1]*bv[2] + p13*bu[1]*bv[3] +
			//p20*bu[2]*bv[0] + p21*bu[2]*bv[1] + p22*bu[2]*bv[2] + p23*bu[2]*bv[3] +
			//p30*bu[3]*bv[0] + p31*bu[3]*bv[1] + p32*bu[3]*bv[2] + p33*bu[3]*bv[3];

			newStr.append("vec4 pos = \n");
			for (int y = 0; y < tesY; ++y) {
				for (int x = 0; x < tesX; ++x) {
					if (x + y * tesX >= maxPoints) break;

					if (x != 0 || y != 0) newStr.append(" + ");

					newStr.append(std::format("p{}{}*bu[{}]*bv[{}]", y, x, x, y));
				}
			}
			newStr.append(";\n");

			// The partial derivatives
			//vec4 du =
			//p00*dbu[0]*bv[0] + p01*dbu[0]*bv[1] + p02*dbu[0]*bv[2] + p03*dbu[0]*bv[3] +
			//p10*dbu[1]*bv[0] + p11*dbu[1]*bv[1] + p12*dbu[1]*bv[2] + p13*dbu[1]*bv[3] +
			//p20*dbu[2]*bv[0] + p21*dbu[2]*bv[1] + p22*dbu[2]*bv[2] + p23*dbu[2]*bv[3] + 
			//p30*dbu[3]*bv[0] + p31*dbu[3]*bv[1] + p32*dbu[3]*bv[2] + p33*dbu[3]*bv[3];

			newStr.append("vec4 du = \n");
			for (int x = 0; x < tesX; ++x) {
				for (int y = 0; y < tesY; ++y) {
					if (x + y * tesX >= maxPoints) break;

					if (x != 0 || y != 0) newStr.append(" + ");

					newStr.append(std::format("p{}{}*dbu[{}]*bv[{}]", y, x, x, y));
				}
			}
			newStr.append(";\n");

			//vec4 dv =
			//p00*bu[0]*dbv[0] + p01*bu[0]*dbv[1] + p02*bu[0]*dbv[2] + p03*bu[0]*dbv[3] +
			//p10*bu[1]*dbv[0] + p11*bu[1]*dbv[1] + p12*bu[1]*dbv[2] + p13*bu[1]*dbv[3] +
			//p20*bu[2]*dbv[0] + p21*bu[2]*dbv[1] + p22*bu[2]*dbv[2] + p23*bu[2]*dbv[3] +
			//p30*bu[3]*dbv[0] + p31*bu[3]*dbv[1] + p32*bu[3]*dbv[2] + p33*bu[3]*dbv[3];

			newStr.append("vec4 dv = \n");
			for (int y = 0; y < tesY; ++y) {
				for (int x = 0; x < tesX; ++x) {
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