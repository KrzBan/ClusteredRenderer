#include "Renderer.hpp"

#include <Components.hpp>
#include <Entity.hpp>
#include <Utils/FileUtils.hpp>

struct ClusterSSBO {
	uint32 lightOffset;
	uint32 pointLightCount;
};

struct [[nodiscard]] glEnableScoped {
	glEnableScoped(uint32 option) : m_Option{ option } {
		glEnable(m_Option);
	}
	~glEnableScoped() {
		glDisable(m_Option);
	}

private:
	uint32 m_Option;
};

Renderer::Renderer() {
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glCullFace(GL_BACK);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

	// Create Uniform Buffer Object for Camera Projection+View
	glGenBuffers(1, &uboMatricies);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::uvec4) + sizeof(glm::vec2) + 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2 * sizeof(glm::mat4) + sizeof(glm::uvec4) + sizeof(glm::vec2) + 2 * sizeof(float));

	// Create SSBO for lights
	glGenBuffers(1, &ssboLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLights);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboLights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightSSBO) * 1000, nullptr, GL_DYNAMIC_DRAW);

	// Create SSBO for Clusters
	glGenBuffers(1, &ssboClusters);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboClusters);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboClusters);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ssboLightIndices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLightIndices);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboLightIndices);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Load Postprocess
	const auto LoadShader = [](ShaderRenderInfo& renderInfo, 
		std::string vertexPath, std::string fragmentPath, 
		std::optional<std::string> geometryPath, 
		std::string name) {
		ShaderAsset shaderAsset{};
		shaderAsset.vertex = std::make_shared<ShaderSourceAsset>();
		shaderAsset.vertex->LoadAsset(std::filesystem::path(vertexPath));
		shaderAsset.fragment = std::make_shared<ShaderSourceAsset>();
		shaderAsset.fragment->LoadAsset(std::filesystem::path(fragmentPath));
		if (geometryPath.has_value()) {
			shaderAsset.geometry = std::make_shared<ShaderSourceAsset>();
			shaderAsset.geometry->LoadAsset(std::filesystem::path(geometryPath.value()));
		}
		{
			auto compileRes = CompileShader(shaderAsset);
			if (compileRes.has_value()) {
				renderInfo = std::move(compileRes.value());
			}
			else {
				spdlog::error("[Renderrer::Renderrer: Couldn't compile {} shader", name);
				spdlog::error("{}", compileRes.error());
			}
		}
	};
	LoadShader(postprocessShaderRenderInfo, 
		RESOURCES_DIR "shaders/postprocess.vert", 
		RESOURCES_DIR "shaders/postprocess.frag", {},
		"postprocess");
	LoadShader(gridShaderRenderInfo,
		RESOURCES_DIR "shaders/grid.vert",
		RESOURCES_DIR "shaders/grid.frag", {},
		"grid");
	LoadShader(hdrToCubemapsRenderInfo,
		RESOURCES_DIR "shaders/hdrToCubemaps.vert",
		RESOURCES_DIR "shaders/hdrToCubemaps.frag", {},
		"hdrToCubemaps");
	LoadShader(skyboxShaderRenderInfo,
		RESOURCES_DIR "shaders/skybox.vert",
		RESOURCES_DIR "shaders/skybox.frag", {},
		"skybox");
	LoadShader(irradianceShaderRenderInfo,
		RESOURCES_DIR "shaders/irradiance.vert",
		RESOURCES_DIR "shaders/irradiance.frag", {},
		"irradiance");
	LoadShader(prefilterShaderRenderInfo,
		RESOURCES_DIR "shaders/prefilter.vert",
		RESOURCES_DIR "shaders/prefilter.frag", {},
		"prefilter");
	LoadShader(brdfShaderRenderInfo,
		RESOURCES_DIR "shaders/brdf.vert",
		RESOURCES_DIR "shaders/brdf.frag", {},
		"brdf");
	
	const char whiteTexData[] = { 255, 255, 255, 255 };
	glGenTextures(1, &defaultTextureRenderInfo.textureId);
	glBindTexture(GL_TEXTURE_2D, defaultTextureRenderInfo.textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteTexData);

	// Additional FBO for skybox IBL
	glGenFramebuffers(1, &skyboxFbo);
	glGenRenderbuffers(1, &skyboxRbo);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, skyboxRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, skyboxRbo);

	// BRDF
	glGenTextures(1, &brdfLUTId);
	glBindTexture(GL_TEXTURE_2D, brdfLUTId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, skyboxRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTId, 0);

	glViewport(0, 0, 512, 512);
	glUseProgram(brdfShaderRenderInfo.programId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	DrawScreenQuad();
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 2D Polyhedral Bounds of a Clipped, Perspective-Projected 3D Sphere. Michael Mara, Morgan McGuire. 2013
bool ProjectSphereView(const glm::vec3& c, float r, float znear, float P00, float P11, glm::vec4& aabb) {
	if ( c.z < (znear + r))
		return false;

	glm::vec3 cr = c * r;
	float czr2 = c.z * c.z - r * r;

	float vx = std::sqrt(c.x * c.x + czr2);
	float minx = (vx * c.x - cr.z) / (vx * c.z + cr.x);
	float maxx = (vx * c.x + cr.z) / (vx * c.z - cr.x);

	float vy = std::sqrt(c.y * c.y + czr2);
	float miny = (vy * c.y - cr.z) / (vy * c.z + cr.y);
	float maxy = (vy * c.y + cr.z) / (vy * c.z - cr.y);

	aabb = glm::vec4(minx * P00, miny * P11, maxx * P00, maxy * P11);
	// clip space -> uv space
	aabb = glm::vec4(aabb.x, aabb.w, aabb.z, aabb.y) * glm::vec4(0.5f, -0.5f, 0.5f, -0.5f) + glm::vec4(0.5f);

	return true;
}

void UpdateClipRegionRoot(float nc, float lc, float lz, float Radius, float CameraScale, float& ClipMin, float& ClipMax) {
	float nz = (Radius - nc * lc) / lz;
	float pz = (lc * lc + lz * lz - Radius * Radius) / (lz - (nz / nc) * lc);
	if (pz > 0.0f) {
		float c = -nz * CameraScale / nc;
		if (nc > 0.0f)
			ClipMin = std::max(ClipMin, c);
		else
			ClipMax = std::min(ClipMax, c);
	}
}

void UpdateClipRegion(float lc, float lz, float Radius, float CameraScale, float& ClipMin, float& ClipMax) {
	float rSq = Radius * Radius;
	float lcSqPluslzSq = lc * lc + lz * lz;
	float d = rSq * lc * lc - lcSqPluslzSq * (rSq - lz * lz);
	if (d > 0.0f) {
		float a = Radius * lc;
		float b = sqrt(d);
		float nx0 = (a + b) / lcSqPluslzSq;
		float nx1 = (a - b) / lcSqPluslzSq;
		UpdateClipRegionRoot(nx0, lc, lz, Radius, CameraScale, ClipMin, ClipMax);
		UpdateClipRegionRoot(nx1, lc, lz, Radius, CameraScale, ClipMin, ClipMax);
	}
}

void ComputeClipRegion(const glm::vec3& Center, float Radius, glm::vec4& ClipRegion, float CameraNear, const glm::mat4& Projection) {
	ClipRegion = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
	if ((Center.z + Radius) >= CameraNear) {
		glm::vec2 ClipMin = glm::vec2(-1.0f, -1.0f);
		glm::vec2 ClipMax = glm::vec2(+1.0f, +1.0f);
		UpdateClipRegion(Center.x, Center.z, Radius, Projection[0].x, ClipMin.x, ClipMax.x);
		UpdateClipRegion(Center.y, Center.z, Radius, Projection[1].y, ClipMin.y, ClipMax.y);
		ClipRegion = glm::vec4(ClipMin, ClipMax);
	}
}

void ComputeBoundingBox(const glm::vec3& Center, float Radius, glm::vec4& Bounds, float CameraNear, const glm::mat4& Projection) {
	ComputeClipRegion(Center, Radius, Bounds, CameraNear, Projection);
	Bounds = 0.5f * glm::vec4(Bounds.x, -Bounds.y, Bounds.z, -Bounds.w) + 0.5f;
	Bounds[1] = Bounds[1] * -1.0f + 1.0f;
	Bounds[3] = Bounds[3] * -1.0f + 1.0f;
}

struct ClusterInfo {
	uint32 numLights;
	uint32 offset;
};

void Renderer::UpdateLights(Scene& scene, const Camera& camera, const glm::mat4& view) {
	// Set lights
	auto startLightGather = std::chrono::steady_clock::now();

	lights.clear();
	lightClusteredInfos.clear();

	auto transLight = scene.m_Registry.group(entt::get<TransformComponent, LightComponent>);
	for (auto entity : transLight) {
		auto [transform, light] = transLight.get<TransformComponent, LightComponent>(entity);

		if (light.isActive) {
			glm::vec4 aabb{};
			auto viewSpace = view * transform.GetTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			viewSpace.z = -viewSpace.z;
			ComputeBoundingBox(viewSpace, light.range, aabb,
				camera.m_NearClip, camera.m_Projection);
				
			// (upper left, lower right) point on screen
			const auto lightClusterInfo = LightClusteredInfo{
				light, { aabb[0], aabb[2] }, { aabb[1], aabb[3] },
				{ viewSpace.z - light.range, viewSpace.z + light.range }, lights.size()
			};

			if (aabb.x >= 1.0f || aabb.z <= 0.0f ||
				aabb.y >= 1.0f || aabb.w <= 0.0f)
				continue;

			lightClusteredInfos.push_back(lightClusterInfo);
			
			lights.push_back({ glm::vec4(transform.Translation, 1.0f),
				light.ambient, light.diffuse, light.specular,
				light.ambientStrength, light.diffuseStrength, light.specularStrength,
				light.range });
		}
	}

	auto endLightGather = std::chrono::steady_clock::now();
	spdlog::info("Light Gather: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(endLightGather - startLightGather).count());

	const auto zFunc = [](float z, float numSlices, float nearPlane, float farPlane) -> float {
		return nearPlane * std::powf(farPlane/nearPlane, z/numSlices);
	};

	std::vector<ClusterInfo> clusterInfos{numClusters.x * numClusters.y * numClusters.z};

	lightIndicesPerZ.resize(numClusters.z);

	std::vector<uint32> zIndexes(numClusters.z);
	std::iota(zIndexes.begin(), zIndexes.end(), 0);

	auto startClusterAssignment = std::chrono::steady_clock::now();

	std::for_each(std::execution::par_unseq, zIndexes.begin(), zIndexes.end(), [&](uint32 z) {
		glm::vec2 zExtents = { zFunc(z, numClusters.z, camera.m_NearClip, camera.m_FarClip),
			zFunc((z + 1), numClusters.z, camera.m_NearClip, camera.m_FarClip) };

		lightIndicesPerZ[z].vector.clear();

		for (size_t y = 0; y < numClusters.y; ++y) {
			glm::vec2 yExtents = { y * 1.0f / numClusters.y, (y + 1) * 1.0f / numClusters.y };
			for (size_t x = 0; x < numClusters.x; ++x) {
				glm::vec2 xExtents = { x * 1.0f / numClusters.x, (x + 1) * 1.0f / numClusters.x };

				size_t clusterId = x + y * numClusters.x + z * numClusters.x * numClusters.y;

				uint32 numLights = 0;
				uint32 offset = lightIndicesPerZ[z].vector.size();

				for (const auto& lightClusterInfo : lightClusteredInfos) {

					if (
						(xExtents.x <= lightClusterInfo.xExtents.y && lightClusterInfo.xExtents.x <= xExtents.y) &&
						(yExtents.x <= lightClusterInfo.yExtents.y && lightClusterInfo.yExtents.x <= yExtents.y) &&
						(zExtents.x <= lightClusterInfo.zExtents.y && lightClusterInfo.zExtents.x <= zExtents.y)) {
						// Light in cluster
						lightIndicesPerZ[z].vector.push_back(lightClusterInfo.lightId);
						++numLights;
					}
				}

				clusterInfos[clusterId] = { numLights, offset };
			}
		}
	});

	auto endClusterAssignment = std::chrono::steady_clock::now();
	spdlog::info("Cluser Assignment: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(endClusterAssignment - startClusterAssignment).count());

	std::vector<uint32> lightIndices( std::accumulate(lightIndicesPerZ.begin(), lightIndicesPerZ.end(), size_t{ 0 }, [](size_t lhs, auto vec) { return lhs + vec.vector.size(); }) );
	size_t offset = 0;
	for (const auto& vec : lightIndicesPerZ) {
		std::copy(vec.vector.begin(), vec.vector.end(), std::next(lightIndices.begin(), offset));
		offset += vec.vector.size();
	}

	uint32 accumulatedOffset = 0;
	for (uint32 z = 1; z < numClusters.z; ++z) {
		accumulatedOffset += lightIndicesPerZ[z - 1].vector.size();
		
		for (uint32 id = 0; id < numClusters.x * numClusters.y; ++id) {
			uint32 clusterId = id + z * numClusters.x * numClusters.y;
			clusterInfos[clusterId].offset += accumulatedOffset;
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLights);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboLights);
	if (lights.size())
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightSSBO) * lights.size(), lights.data(), GL_DYNAMIC_DRAW);
	else {
		// Absolutely insane, when setting SSBO data to nothing, size = 0, the length of dynamic array is set to max
		// As a workaround, we have to set buffer's data to something larger than 0, but smaller then sizeof(LightSSBO)
		int invalid = 0xBADBAD;
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), &invalid, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboClusters);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboClusters);
	if (clusterInfos.size())
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ClusterInfo) * clusterInfos.size(), clusterInfos.data(), GL_DYNAMIC_DRAW);
	else {
		char invalid = 0xBD;
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(char), &invalid, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLightIndices);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboLightIndices);
	if (lightIndices.size())
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32) * lightIndices.size(), lightIndices.data(), GL_DYNAMIC_DRAW);
	else {
		char invalid = 0xBD;
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(char), &invalid, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::RenderMeshes(Scene& scene) {
	auto group = scene.m_Registry.group(entt::get<TransformComponent, MeshRendererComponent>);
	// Non Transparent
	for (auto entity : group) {
		auto [transform, meshRenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);

		if (meshRenderer.mesh == nullptr || not meshRenderer.isActive)
			continue;

		const auto meshResult = PrepareMesh(*meshRenderer.mesh);

		for (const auto& [id, submeshRenderInfo] : meshResult) {
			if (not meshRenderer.materials.contains(id)) {
				continue;
			}
			const auto& material = [&]() {
				if (materialOverride == nullptr)
					return meshRenderer.materials[id];
				return materialOverride;
			}();

			if (material == nullptr || material->shaderAsset == nullptr)
				continue;

			if (material->shaderAsset->isTransparent)
				continue;

			const auto shaderResult = PrepareShader(*material->shaderAsset);
			if (shaderResult == nullptr) {
				continue;
			}

			// Bind uniforms
			glUseProgram(shaderResult->programId);
			const auto modelUniformLocation = glGetUniformLocation(shaderResult->programId, "model");
			glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(transform.GetTransform()));

			// Bind Irradiance
			if (irradianceId != 0) {
				constexpr int irradianceMapLocation = 6;
				int irradianceLocation = glGetUniformLocation(shaderResult->programId, "irradianceMap");

				if (irradianceLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + irradianceMapLocation);
					glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);

					glUniform1i(irradianceLocation, irradianceMapLocation);
				}
			}

			// Bind prefilterMap
			if (prefilterMapId != 0) {
				constexpr int prefilterMapLayout = 7;
				int prefilterMapLocation = glGetUniformLocation(shaderResult->programId, "prefilterMap");

				if (prefilterMapLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + prefilterMapLayout);
					glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMapId);

					glUniform1i(prefilterMapLocation, prefilterMapLayout);
				}
			}

			// Bind brdfLUT
			if (brdfLUTId != 0) {
				constexpr int brdfLUTMapLocation = 8;
				int brdfLUTLocation = glGetUniformLocation(shaderResult->programId, "brdfLUT");

				if (brdfLUTLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + brdfLUTMapLocation);
					glBindTexture(GL_TEXTURE_2D, brdfLUTId);

					glUniform1i(brdfLUTLocation, brdfLUTMapLocation);
				}
			}

			uint32 textureSlot = 0;
			for (const auto& uniform : material->uniforms) {
				BindUniform(shaderResult->programId, uniform, textureSlot);
			}

			// Render
			glBindVertexArray(submeshRenderInfo->vao);
			glDrawElements(GL_TRIANGLES, submeshRenderInfo->nIndicies, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	// Transparent
	for (auto entity : group) {
		auto [transform, meshRenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);

		if (meshRenderer.mesh == nullptr || not meshRenderer.isActive)
			continue;

		const auto meshResult = PrepareMesh(*meshRenderer.mesh);

		for (const auto& [id, submeshRenderInfo] : meshResult) {
			if (not meshRenderer.materials.contains(id)) {
				continue;
			}
			const auto& material = [&]() {
				if (materialOverride == nullptr)
					return meshRenderer.materials[id];
				return materialOverride;
			}();

			if (material == nullptr || material->shaderAsset == nullptr)
				continue;

			if (not material->shaderAsset->isTransparent)
				continue;

			const auto shaderResult = PrepareShader(*material->shaderAsset);
			if (shaderResult == nullptr) {
				continue;
			}

			// Bind uniforms
			glUseProgram(shaderResult->programId);
			const auto modelUniformLocation = glGetUniformLocation(shaderResult->programId, "model");
			glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(transform.GetTransform()));

			// Bind Irradiance
			if (irradianceId != 0) {
				constexpr int irradianceMapLocation = 6;
				int irradianceLocation = glGetUniformLocation(shaderResult->programId, "irradianceMap");

				if (irradianceLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + irradianceMapLocation);
					glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);

					glUniform1i(irradianceLocation, irradianceMapLocation);
				}
			}

			// Bind prefilterMap
			if (prefilterMapId != 0) {
				constexpr int prefilterMapLayout = 7;
				int prefilterMapLocation = glGetUniformLocation(shaderResult->programId, "prefilterMap");

				if (prefilterMapLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + prefilterMapLayout);
					glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMapId);

					glUniform1i(prefilterMapLocation, prefilterMapLayout);
				}
			}

			// Bind brdfLUT
			if (brdfLUTId != 0) {
				constexpr int brdfLUTMapLocation = 8;
				int brdfLUTLocation = glGetUniformLocation(shaderResult->programId, "brdfLUT");

				if (brdfLUTLocation != -1) {
					glActiveTexture(GL_TEXTURE0 + brdfLUTMapLocation);
					glBindTexture(GL_TEXTURE_2D, brdfLUTId);

					glUniform1i(brdfLUTLocation, brdfLUTMapLocation);
				}
			}

			uint32 textureSlot = 0;
			for (const auto& uniform : material->uniforms) {
				BindUniform(shaderResult->programId, uniform, textureSlot);
			}

			// Render
			glBindVertexArray(submeshRenderInfo->vao);
			glDrawElements(GL_TRIANGLES, submeshRenderInfo->nIndicies, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void Renderer::RenderGrid() {
	static unsigned int VAO{ 0 };
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
	}

	glUseProgram(gridShaderRenderInfo.programId);

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
	glUseProgram(0);
}

void Renderer::Postprocess() {
	postprocessFbo.Bind();
	glUseProgram(postprocessShaderRenderInfo.programId);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, hdrFbo.GetColorAttachmentTextureID());

	glUniform1i(glGetUniformLocation(postprocessShaderRenderInfo.programId, "hdrBuffer"), 0);

	glUniform1f(glGetUniformLocation(postprocessShaderRenderInfo.programId, "uGamma"), gamma);
	glUniform1f(glGetUniformLocation(postprocessShaderRenderInfo.programId, "uExposure"), exposure);

	glDisable(GL_DEPTH_TEST);
	DrawScreenQuad();
	glEnable(GL_DEPTH_TEST);

	postprocessFbo.Unbind();
}

void Renderer::RenderSkybox() {

	if (cubemapId == 0)
		return;

	glUseProgram(skyboxShaderRenderInfo.programId);

	glActiveTexture(GL_TEXTURE0);
	if (not showIrradiance)
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
	else
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);
	DrawCube();

}

void Renderer::HdrToCubemaps() {
	if (cubemapId != 0) {
		glDeleteTextures(1, &cubemapId);
		cubemapId = 0;
	}
	if (irradianceId != 0) {
		glDeleteTextures(1, &irradianceId);
		irradianceId = 0;
	}
	if (prefilterMapId != 0) {
		glDeleteTextures(1, &prefilterMapId);
		prefilterMapId = 0;
	}
	
	if (hdrSkybox == nullptr) {
		return;
	}

	if (cubemapId == 0) {
		glGenTextures(1, &cubemapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
		for (unsigned int i = 0; i < 6; ++i) {
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	
	static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	static glm::mat4 captureViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	glUseProgram(hdrToCubemapsRenderInfo.programId);
	
	glUniform1i(glGetUniformLocation(hdrToCubemapsRenderInfo.programId, "hdrMap"), 0);
	glUniformMatrix4fv(
		glGetUniformLocation(hdrToCubemapsRenderInfo.programId, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	auto textureResult = hdrSkybox != nullptr ? PrepareTexture2D(*hdrSkybox) : nullptr;
	if (textureResult == nullptr)
		textureResult = &defaultTextureRenderInfo;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureResult->textureId);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, skyboxRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glViewport(0, 0, 512, 512);
	for (unsigned int i = 0; i < 6; ++i) {
		glUniformMatrix4fv(
			glGetUniformLocation(hdrToCubemapsRenderInfo.programId, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawCube();
	}

	// Generate MipMaps for cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Irradiance
	if (irradianceId == 0) {
		glGenTextures(1, &irradianceId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
				GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	
	glUseProgram(irradianceShaderRenderInfo.programId);

	glUniform1i(glGetUniformLocation(irradianceShaderRenderInfo.programId, "environmentMap"), 0);
	glUniformMatrix4fv(
		glGetUniformLocation(irradianceShaderRenderInfo.programId, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, skyboxRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glViewport(0, 0, 32, 32);
	for (unsigned int i = 0; i < 6; ++i) {
		glUniformMatrix4fv(
			glGetUniformLocation(irradianceShaderRenderInfo.programId, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawCube();
	}

	// Specular IBL
	if (prefilterMapId == 0) {
		glGenTextures(1, &prefilterMapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMapId);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glUseProgram(prefilterShaderRenderInfo.programId);
	glUniform1i(glGetUniformLocation(prefilterShaderRenderInfo.programId, "environmentMap"), 0);
	glUniformMatrix4fv(
		glGetUniformLocation(prefilterShaderRenderInfo.programId, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFbo);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, skyboxRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(prefilterShaderRenderInfo.programId, "roughness"), roughness);

		for (unsigned int i = 0; i < 6; ++i) {
			glUniformMatrix4fv(
				glGetUniformLocation(prefilterShaderRenderInfo.programId, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMapId, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			DrawCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderScene(Scene& scene, const Camera& camera, const glm::mat4& transform) {
	hdrFbo.Bind();

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set camera matricies
	glm::vec2 windowSize = { hdrFbo.GetWidth(), hdrFbo.GetHeight() };
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetProjection()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(transform));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::uvec4), glm::value_ptr(numClusters));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::uvec4), sizeof(glm::vec2), glm::value_ptr(windowSize));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::uvec4) + sizeof(glm::vec2), sizeof(float), &camera.m_NearClip);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::uvec4) + sizeof(glm::vec2) + sizeof(float), sizeof(float), &camera.m_FarClip);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	UpdateLights(scene, camera, transform);
	RenderMeshes(scene);

	RenderSkybox();

	if (renderGrid) {
		RenderGrid();
	}
	Postprocess();
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
	
				const auto textureResult = sampler2D.textureAsset != nullptr ?
					PrepareTexture2D(*sampler2D.textureAsset) : 
					&defaultTextureRenderInfo;

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


std::unordered_map<kb::UUID, const MeshRenderInfo*> Renderer::PrepareMesh(MeshAsset& mesh) {
	if (mesh.isDirty) {
		mesh.isDirty = false;
		// Update
		for (const auto& submesh : mesh.submeshes) {
			MeshRenderInfo meshRenderInfo;
			meshRenderInfo.nIndicies = submesh.indices.size();

			glGenBuffers(1, &meshRenderInfo.vbo);
			glGenBuffers(1, &meshRenderInfo.ebo);
			glGenVertexArrays(1, &meshRenderInfo.vao);

			glBindVertexArray(meshRenderInfo.vao);

			glBindBuffer(GL_ARRAY_BUFFER, meshRenderInfo.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * submesh.vertices.size(), submesh.vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRenderInfo.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * submesh.indices.size(), submesh.indices.data(), GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			// vertex tangent
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
			// vertex biTangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));
			// vertex texture coords
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

			glBindVertexArray(0);

			m_Meshes[submesh.submeshId] = std::move(meshRenderInfo);
		}
	}

	std::unordered_map<kb::UUID, const MeshRenderInfo*> submeshRenderInfos{};

	for (const auto& submesh : mesh.submeshes) {
		if (not m_Meshes.contains(submesh.submeshId)) {
			spdlog::error("[Renderer::PrepareMesh] Submesh id {} is missing.", submesh.submeshId);
			continue;
		}
		submeshRenderInfos[submesh.submeshId] = &m_Meshes[submesh.submeshId];
	}

	return submeshRenderInfos;
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
			texture.GetWidth(), texture.GetHeight(), 0, texture.m_DataFormat, texture.m_DataType, texture.m_Data.data());
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

		static const std::string utilsStr = LoadFileText(RESOURCES_DIR "shaders/utils.glsl");
		const char* ptr[] = {
			utilsStr.c_str(),
			shaderSource.source.c_str()
		};
		glShaderSource(shader, 2, ptr, NULL);
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

	GLuint clustersShaderStorageBlockIndex = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, "s_Clusters");
	if (clustersShaderStorageBlockIndex == GL_INVALID_INDEX) {
		spdlog::info("[Renderer::CompileShader] Shader does not contain Shader Storage Block Index: s_Clusters");
	}
	else {
		glShaderStorageBlockBinding(programId, clustersShaderStorageBlockIndex, 1);
	}

	GLuint lightIndicesShaderStorageBlockIndex = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, "s_LightIndices");
	if (lightIndicesShaderStorageBlockIndex == GL_INVALID_INDEX) {
		spdlog::info("[Renderer::CompileShader] Shader does not contain Shader Storage Block Index: s_LightIndices");
	}
	else {
		glShaderStorageBlockBinding(programId, lightIndicesShaderStorageBlockIndex, 2);
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
	properties.push_back(GL_BLOCK_INDEX);
	std::vector<GLint> values(properties.size());

	for (int attrib = 0; attrib < numActiveUniforms; ++attrib) {
		glGetProgramResourceiv(shaderInfo.programId, GL_UNIFORM, attrib, properties.size(),
			&properties[0], values.size(), NULL, &values[0]);

		// Skip any uniforms that are in a block.
		if (values[3] != -1)
			continue;

		nameData.resize(values[0]);
		glGetProgramResourceName(shaderInfo.programId, GL_UNIFORM, attrib, nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0], nameData.size() - 1);

		if (name == "model" || name == "projection" || name == "view" 
			|| name == "irradianceMap" || name == "brdfLUT" || name == "prefilterMap") // implicit for all models
			continue;

		Uniform uniform;
		uniform.name = name;
		uniform.uniform = GlTypeToUniformVariant(values[1]);

		uniforms.push_back(uniform);
	}

	return uniforms;
}

void Renderer::DrawScreenQuad() {
	static uint32 quadVAO = 0;
	static uint32 quadVBO = 0;
	if (quadVAO == 0) {
		float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::DrawCube() {
	// initialize (if necessary)
	static uint32 cubeVAO = 0;
	static uint32 cubeVBO = 0;
	if (cubeVAO == 0) {
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
																// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	  // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
