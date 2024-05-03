#pragma once

#include <Core.hpp>

#include <Scene.hpp>
#include <Assets/AssetTypes/MeshAsset.hpp>
#include <Assets/AssetTypes/ShaderAsset.hpp>
#include <Assets/AssetTypes/MaterialAsset.hpp>

#include "Framebuffer.hpp"
#include "OpenGL/MeshRenderInfo.hpp"
#include "OpenGL/ShaderRenderInfo.hpp"
#include "OpenGL/Texture2DRenderInfo.hpp"

#include "UniformTypes.hpp"

class Renderer {
public:
	Renderer();
	void RenderScene(Scene& scene, const Camera& camera, const glm::mat4& transform);

	// Returns optional error
	static void CompileShaderOneTime(ShaderAsset& shader);
	static bool CheckShaderValidity(const ShaderAsset& shader);

	static std::vector<Uniform> QueryShaderUniforms(const ShaderAsset& shader);

private:
	static std::expected<ShaderRenderInfo, std::string> CompileShader(const ShaderAsset& shader);

	std::unordered_map<kb::UUID, const MeshRenderInfo*> PrepareMesh(MeshAsset& mesh);
	const ShaderRenderInfo* PrepareShader(ShaderAsset& shader);
	const Texture2DRenderInfo* PrepareTexture2D(Texture2DAsset& texture);

	void BindUniform(GLuint shaderId, const Uniform& uniform, uint32& textureSlot);
	void DrawScreenQuad();
	void DrawCube();

	void UpdateLights(Scene& scene, const Camera& camera, const glm::mat4& view);
	void RenderMeshes(Scene& scene);
	void RenderGrid();
	void Postprocess();
	void RenderSkybox();

public:
	void HdrToCubemaps();

public:
	Framebuffer hdrFbo{ 1, 1, true };
	Framebuffer postprocessFbo{ 1, 1, false };

	Shared<Texture2DAsset> hdrSkybox;
	Shared<MaterialAsset> materialOverride;

	bool renderGrid = true;
	bool showIrradiance = false;

	float exposure = 1.0f;
	float gamma = 2.2f;

	bool wireframeOverride = false;
	glm::uvec4 numClusters{ 16, 9, 24, 1 };

private:
	uint32 uboMatricies;
	uint32 ssboLights;

	uint32 ssboClusters;
	uint32 ssboLightIndices;

	std::unordered_map<kb::UUID, MeshRenderInfo> m_Meshes;
	std::unordered_map<kb::UUID, ShaderRenderInfo> m_Shaders;
	std::unordered_map<kb::UUID, Texture2DRenderInfo> m_Textures;

	ShaderRenderInfo gridShaderRenderInfo;
	ShaderRenderInfo postprocessShaderRenderInfo;
	ShaderRenderInfo hdrToCubemapsRenderInfo;
	ShaderRenderInfo skyboxShaderRenderInfo;
	ShaderRenderInfo irradianceShaderRenderInfo;
	ShaderRenderInfo prefilterShaderRenderInfo;
	ShaderRenderInfo brdfShaderRenderInfo;

	uint32 skyboxFbo = 0;
	uint32 skyboxRbo = 0;
	uint32 cubemapId = 0;
	uint32 irradianceId = 0;
	uint32 prefilterMapId = 0;
	uint32 brdfLUTId = 0;

	Texture2DRenderInfo defaultTextureRenderInfo;
};