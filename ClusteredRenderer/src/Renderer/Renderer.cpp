#include "Renderer.hpp"

#include <Components.hpp>

Renderer::Renderer() {
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create Uniform Buffer Object for Camera Projection+View
	glGenBuffers(1, &uboMatricies);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2 * sizeof(glm::mat4));

	// Load Shader for Editor Grid
	ShaderAsset gridShaderAsset{};
	gridShaderAsset.vertex = std::make_shared<ShaderSourceAsset>();
	gridShaderAsset.vertex->LoadAsset(std::filesystem::path(RESOURCES_DIR "shaders/grid.vert"));
	gridShaderAsset.fragment = std::make_shared<ShaderSourceAsset>();
	gridShaderAsset.fragment->LoadAsset(std::filesystem::path(RESOURCES_DIR "shaders/grid.frag"));

	auto compileRes = CompileShader(gridShaderAsset);
	if (compileRes.has_value()) {
		gridShaderRenderInfo = std::move(compileRes.value());
	}
	else {
		spdlog::error("[Renderrer::Renderrer: Couldn't compile grid shader");
		spdlog::error("{}", compileRes.error());
	}
}

void Renderer::RenderScene(const Scene& scene, const Camera& camera, const glm::mat4& transform) {
	framebuffer.Bind();

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set camera matricies
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetProjection()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(transform));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Render Editor Grid
	static unsigned int VAO{ 0 };
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO); 
	}
		 
	glUseProgram(gridShaderRenderInfo.programId);
	glBindVertexArray(VAO); 
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
	glUseProgram(0);

	framebuffer.Unbind();
}

std::expected<ShaderRenderInfo, std::string> Renderer::CompileShader(const ShaderAsset& shader) {
	std::string error = "";

	if (not CheckShaderValidity(shader)) {
		return std::unexpected{ "Invalid" };
	}

	if (shader.vertex == nullptr) {
		error += "Vertex shader is mandatory.\n";
	}
	if (shader.fragment == nullptr) {
		error += "Fragment shader is mandatory.\n";
	}
	if (error != "") {
		return std::unexpected{ error };
	}

	const auto programId = glCreateProgram();
	ShaderRenderInfo shaderRenderInfo{};
	shaderRenderInfo.programId = programId;

	const auto compileAndLink = [&error, &programId](int shaderType, ShaderSourceAsset& shaderSource) {
		const auto shader = glCreateShader(shaderType);
		const char* ptr = shaderSource.source.c_str();
		glShaderSource(shader, 1, &ptr, NULL);
		glCompileShader(shader);

		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (not success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			error += infoLog;
			error += '\n';
		};
		glAttachShader(programId, shader);
		glDeleteShader(shader);
	};
	
	compileAndLink(GL_VERTEX_SHADER, *shader.vertex);
	if (shader.tesselation_control != nullptr)
		compileAndLink(GL_TESS_CONTROL_SHADER, *shader.tesselation_control);
	if (shader.tesselation_evaluation != nullptr)
		compileAndLink(GL_TESS_EVALUATION_SHADER, *shader.tesselation_evaluation);
	if (shader.geometry != nullptr)
		compileAndLink(GL_GEOMETRY_SHADER, *shader.geometry);
	compileAndLink(GL_FRAGMENT_SHADER, *shader.fragment);
	if (shader.compute != nullptr)
		compileAndLink(GL_COMPUTE_SHADER, *shader.compute);

	int success;
	char infoLog[512];
	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (not success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		error += infoLog;
		error += '\n';
	}

	unsigned int matricesBlockIndex = glGetUniformBlockIndex(programId, "Matrices");
	if (matricesBlockIndex == GL_INVALID_INDEX) {
		spdlog::info("[Renderer::CompileShader] Shader does not contain Uniform Block Index: Matrices");
	}
	else {
		glUniformBlockBinding(programId, matricesBlockIndex, 0);
	}

	if (error == "")
		return shaderRenderInfo;
	else
		return std::unexpected{error};
}


void Renderer::CompileShaderOneTime(ShaderAsset& shader) {
	const auto result = CompileShader(shader);

	shader.status = result.has_value() ? 
		"Compiled successfully" : 
		result.error();
}

bool Renderer::CheckShaderValidity(const ShaderAsset& shader) {
	bool isOk = true;
	const auto checkValid = [&isOk](const Shared<ShaderSourceAsset>& ptr, ShaderSourceType expectedType) {
		if (ptr && ptr->type != expectedType) {
			spdlog::error("[Renderer::CheckShaderValidity] Assigned {} source is of type: {}", magic_enum::enum_name(expectedType), magic_enum::enum_name(ptr->type));
			isOk = false;
		}
	};

	if (shader.vertex == nullptr) {
		spdlog::error("[Renderer::CheckShaderValidity] Vertex shader is mandatory");
		isOk = false;
	}
	if (shader.fragment == nullptr) {
		spdlog::error("[Renderer::CheckShaderValidity] Fragment shader is mandatory");
		isOk = false;
	}

	checkValid(shader.vertex, ShaderSourceType::VERTEX);
	checkValid(shader.tesselation_control, ShaderSourceType::TESSELATION_CONTROL);
	checkValid(shader.tesselation_evaluation, ShaderSourceType::TESSELATION_EVALUTATION);
	checkValid(shader.geometry, ShaderSourceType::GEOMETRY);
	checkValid(shader.fragment, ShaderSourceType::FRAGMENT);
	checkValid(shader.compute, ShaderSourceType::COMPUTE);

	return isOk;
}

std::vector<Uniform> Renderer::QueryShaderUniforms(const ShaderAsset& shader) {
	
	auto result = CompileShader(shader);
	if (result.has_value() == false) {
		spdlog::error("[Renderer::QueryShaderUniforms] Couldn't compile shader");
		spdlog::error(result.error());
		return {};
	}

	const auto shaderInfo = std::move(result.value());

	std::vector<Uniform> uniforms;

	GLint numActiveUniforms = 0;
	glGetProgramInterfaceiv(shaderInfo.programId, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

	std::vector<GLchar> nameData(256);
	std::vector<GLenum> properties;
	properties.push_back(GL_NAME_LENGTH);
	properties.push_back(GL_TYPE);
	properties.push_back(GL_ARRAY_SIZE);
	std::vector<GLint> values(properties.size());

	for (int attrib = 0; attrib < numActiveUniforms; ++attrib) {
		glGetProgramResourceiv(shaderInfo.programId, GL_UNIFORM, attrib, properties.size(),
			&properties[0], values.size(), NULL, &values[0]);

		nameData.resize(values[0]);
		glGetProgramResourceName(shaderInfo.programId, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0], nameData.size() - 1);

		Uniform uniform;
		uniform.name = name;
		uniform.uniform = GlTypeToUniformVariant(values[1]);
	}

	return uniforms;
}
