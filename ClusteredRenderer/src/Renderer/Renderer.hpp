#pragma once

#include <Core.hpp>

#include <Scene.hpp>
#include <Assets/AssetTypes/MeshAsset.hpp>
#include <Assets/AssetTypes/ShaderAsset.hpp>

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

public:
	Framebuffer hdrFbo{ 1, 1, true };
	Framebuffer postprocessFbo{ 1, 1, false };

	uint32_t uboMatricies;
	uint32_t ssboLights;

private:
	std::unordered_map<kb::UUID, MeshRenderInfo> m_Meshes;
	std::unordered_map<kb::UUID, ShaderRenderInfo> m_Shaders;
	std::unordered_map<kb::UUID, Texture2DRenderInfo> m_Textures;

	ShaderRenderInfo gridShaderRenderInfo;
	ShaderRenderInfo postprocessShaderRenderInfo;

	Texture2DRenderInfo defaultTextureRenderInfo;
};