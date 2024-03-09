#pragma once

#include <Core.hpp>

#include <Scene.hpp>
#include <Components.hpp>

#include "Framebuffer.hpp"
#include "OpenGL/ShaderRenderInfo.hpp"

class Renderer {
public:
	Renderer();
	void RenderScene(const Scene& scene, const Camera& camera, const glm::mat4& transform);

	// Returns optional error
	static void CompileShaderOneTime(ShaderAsset& shader);
	static bool CheckShaderValidity(const ShaderAsset& shader);

private:
	static std::expected<ShaderRenderInfo, std::string> CompileShader(ShaderAsset& shader);

public:
	Framebuffer framebuffer{ 1, 1, 0 };

	uint32_t uboCamera;

private:
	std::unordered_map<kb::UUID, ShaderRenderInfo> m_Shaders;
};