#include "Renderer.hpp"

#include <Components.hpp>
#include <Entity.hpp>

struct LightSSBO {
	glm::vec4 position;
	LightComponent lightData;
};

Renderer::Renderer() {
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create Uniform Buffer Object for Camera Projection+View
	glGenBuffers(1, &uboMatricies);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2 * sizeof(glm::mat4));

	// Create SSBO for lights
	glGenBuffers(1, &ssboLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLights);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboLights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightSSBO) * 200, nullptr, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

void Renderer::RenderScene(Scene& scene, const Camera& camera, const glm::mat4& transform) {
	framebuffer.Bind();

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set camera matricies
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetProjection()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(transform));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	{
		// Set lights
		std::vector<LightSSBO> lights;
		auto transLight = scene.m_Registry.group(entt::get<TransformComponent, LightComponent>);
		for (auto entity : transLight) {
			auto [transform, light] = transLight.get<TransformComponent, LightComponent>(entity);

			lights.push_back({ glm::vec4(transform.Translation, 1.0f), light });
		}
		
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLights);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboLights);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightSSBO) * lights.size(), lights.data(), GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	glEnable(GL_FRAMEBUFFER_SRGB); 

	{
		auto group = scene.m_Registry.group(entt::get<TransformComponent, MeshRendererComponent>);
		for (auto entity : group) {
			auto [transform, meshRenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);

			if (meshRenderer.mesh == nullptr)
				continue;
			const auto meshResult = PrepareMesh(*meshRenderer.mesh);
			if (meshResult == nullptr) {
				continue;
			}

			if (meshRenderer.material == nullptr || meshRenderer.material->shaderAsset == nullptr)
				continue;
			const auto shaderResult = PrepareShader(*meshRenderer.material->shaderAsset);
			if (shaderResult == nullptr) {
				continue;
			}

			// Bind uniforms
			glUseProgram(shaderResult->programId);
			const auto modelUniformLocation = glGetUniformLocation(shaderResult->programId, "model");
			glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(transform.GetTransform()));

			uint32 textureSlot = 0;
			for (const auto& uniform : meshRenderer.material->uniforms) {
				BindUniform(shaderResult->programId, uniform, textureSlot);
			}

			// Render
			glBindVertexArray(meshResult->vao);
			glDrawElements(GL_TRIANGLES, meshRenderer.mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	
	// Render Editor Grid
	static unsigned int VAO{ 0 };
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
	}

	glUseProgram(gridShaderRenderInfo.programId);
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
	glUseProgram(0);

	glDisable(GL_FRAMEBUFFER_SRGB); 

	framebuffer.Unbind();
}

void Renderer::BindUniform(GLuint shaderId, const Uniform& uniform, uint32& textureSlot) {

	int uniformLocation = glGetUniformLocation(shaderId, uniform.name.c_str());

	std::visit(
		overload(
			[&](const UniformFloat& value) {	glUniform1f(uniformLocation, value.value); },
			[&](const UniformFloatVec2& vec2) { glUniform2f(uniformLocation, vec2.vec.x, vec2.vec.y); },
			[&](const UniformFloatVec3& vec3) { glUniform3f(uniformLocation, vec3.vec.x, vec3.vec.y, vec3.vec.z); },
			[&](const UniformFloatVec4& vec4) { glUniform4f(uniformLocation, vec4.vec.x, vec4.vec.y, vec4.vec.z, vec4.vec.w); },
			[&](const UniformSampler2D& sampler2D) { 
				if (sampler2D.textureAsset == nullptr)
					return;
				const auto textureResult = PrepareTexture2D(*sampler2D.textureAsset);
				if (textureResult == nullptr) {
					return;
				}

				glActiveTexture(GL_TEXTURE0 + textureSlot);
				glBindTexture(GL_TEXTURE_2D, textureResult->textureId);

				glUniform1i(uniformLocation, textureSlot);
				++textureSlot;
			}),
		uniform.uniform);
}


const MeshRenderInfo* Renderer::PrepareMesh(MeshAsset& mesh) {
	if (mesh.isDirty) {
		mesh.isDirty = false;
		// Update
		MeshRenderInfo meshRenderInfo;

		glGenBuffers(1, &meshRenderInfo.vbo);  
		glGenBuffers(1, &meshRenderInfo.ebo);  
		glGenVertexArrays(1, &meshRenderInfo.vao);  

		glBindVertexArray(meshRenderInfo.vao);

		glBindBuffer(GL_ARRAY_BUFFER, meshRenderInfo.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRenderInfo.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);  

		m_Meshes[mesh.assetId] = std::move(meshRenderInfo);
	}

	if (not m_Meshes.contains(mesh.assetId)) {
		spdlog::error("[Renderer::PrepareMesh] Mesh id {} is missing.", mesh.assetId);
		return nullptr;
	}

	return &m_Meshes[mesh.assetId];
}

const ShaderRenderInfo* Renderer::PrepareShader(ShaderAsset& shader) {
	if (shader.isDirty) {
		shader.isDirty = false;
		
		auto result = CompileShader(shader);
		if (not result.has_value()) {
			spdlog::error("[Renderer::PrepareShader] Error compiling shader.");
			spdlog::error("{}", result.error());
			return nullptr;
		}
		m_Shaders[shader.assetId] = std::move(result.value());
	}

	if (not m_Shaders.contains(shader.assetId)) {
		spdlog::error("[Renderer::PrepareMesh] Mesh id {} is missing.", shader.assetId);
		return nullptr;
	}

	return &m_Shaders[shader.assetId];
}

const Texture2DRenderInfo* Renderer::PrepareTexture2D(Texture2DAsset& texture) {
	if (texture.isDirty) {
		texture.isDirty = false;

		Texture2DRenderInfo renderInfo{};

		glGenTextures(1, &renderInfo.textureId);
		glBindTexture(GL_TEXTURE_2D, renderInfo.textureId);  

		GLuint wrapMode = [&]() {
			switch (texture.wrapMode) {
			case Texture2DWrapMode::CLAMP_EDGE:
				return GL_CLAMP_TO_EDGE;
			case Texture2DWrapMode::CLAMP_BORDER:
				return GL_CLAMP_TO_BORDER;
			case Texture2DWrapMode::MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case Texture2DWrapMode::REPEAT:
				return GL_REPEAT;
			case Texture2DWrapMode::MIRRORED_CLAMP_EDGE:
				return GL_MIRROR_CLAMP_TO_EDGE;
			}
			throw std::runtime_error("[Renderer::PrepareTexture2D] Unknown wrapMode");
		}();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, texture.m_InternalFormat, 
			texture.GetWidth(), texture.GetHeight(), 0, texture.m_DataFormat, GL_UNSIGNED_BYTE, texture.m_Data.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		m_Textures[texture.assetId] = std::move(renderInfo);
	}

	if (not m_Textures.contains(texture.assetId)) {
		spdlog::error("[Renderer::PrepareTexture2D] Texture id {} is missing.", texture.assetId);
		return nullptr;
	}

	return &m_Textures[texture.assetId];
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

	unsigned int matricesBlockIndex = glGetUniformBlockIndex(programId, "s_Camera");
	if (matricesBlockIndex == GL_INVALID_INDEX) {
		spdlog::info("[Renderer::CompileShader] Shader does not contain Uniform Block Index: s_Camera");
	}
	else {
		glUniformBlockBinding(programId, matricesBlockIndex, 0);
	}

	GLuint lightsShaderStorageBlockIndex = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, "s_Lights");
	if (lightsShaderStorageBlockIndex == GL_INVALID_INDEX) {
		spdlog::info("[Renderer::CompileShader] Shader does not contain Shader Storage Block Index: s_Lights");
	}
	else {
		glShaderStorageBlockBinding(programId, lightsShaderStorageBlockIndex, 0);
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
		glGetProgramResourceName(shaderInfo.programId, GL_UNIFORM, attrib, nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0], nameData.size() - 1);

		if (name == "model" || name == "projection" || name == "view") // implicit for all models
			continue;

		Uniform uniform;
		uniform.name = name;
		uniform.uniform = GlTypeToUniformVariant(values[1]);

		uniforms.push_back(uniform);
	}

	return uniforms;
}
