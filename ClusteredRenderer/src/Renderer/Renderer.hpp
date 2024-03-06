#pragma once

#include <Core.hpp>

#include <Scene.hpp>
#include <Components.hpp>

#include "Framebuffer.hpp"

class Renderer {
public:
	Renderer();
	void RenderScene(const Scene& scene, const Camera& camera, const glm::mat4& transform);

public:
	Framebuffer framebuffer{ 1, 1, 0 };

	uint32_t uboCamera;
};