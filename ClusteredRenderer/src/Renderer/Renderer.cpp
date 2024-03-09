#include "Renderer.hpp"

Renderer::Renderer() {
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	glGenBuffers(1, &uboCamera);
	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboCamera, 0, 2 * sizeof(glm::mat4));
}

void Renderer::RenderScene(const Scene& scene, const Camera& camera, const glm::mat4& transform) {
	framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetProjection()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(transform));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	framebuffer.Unbind();
}

std::expected<ShaderRenderInfo, std::string> Renderer::CompileShader(ShaderAsset& shader) {
	return std::unexpected{ std::string{ "xd" } };
}


void Renderer::CompileShaderOneTime(ShaderAsset& shader) {
	std::string error = "";
	
	if (not CheckShaderValidity(shader)) {
		shader.status =  "Invalid";
		return;
	}
		
	if (shader.vertex == nullptr) {
		error += "Vertex shader is mandatory.\n";
	}
	if (shader.fragment == nullptr) {
		error += "Fragment shader is mandatory.\n";
	}
	if (error != "") {
		shader.status = error;
		return;
	}

	const auto programId = glCreateProgram();

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

	glDeleteProgram(programId);

	if (error == "")
		shader.status = "Compiled successfully!";
	else
		shader.status = error;
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