#pragma once

#include <Core.hpp>

#include <Scene.hpp>
#include <Assets/AssetTypes/ShaderAsset.hpp>

#include "Framebuffer.hpp"
#include "OpenGL/ShaderRenderInfo.hpp"
#include "UniformTypes.hpp"

class Renderer {
public:
	Renderer();
	void RenderScene(const Scene& scene, const Camera& camera, const glm::mat4& transform);

	// Returns optional error
	static void CompileShaderOneTime(ShaderAsset& shader);
	static bool CheckShaderValidity(const ShaderAsset& shader);

	static std::vector<Uniform> QueryShaderUniforms(const ShaderAsset& shader);

private:
	static std::expected<ShaderRenderInfo, std::string> CompileShader(const ShaderAsset& shader);

public:
	Framebuffer framebuffer{ 1, 1, 0 };

	uint32_t uboMatricies;

private:
	std::unordered_map<kb::UUID, ShaderRenderInfo> m_Shaders;
	ShaderRenderInfo gridShaderRenderInfo;
};