#include "Renderer.hpp"

Renderer::Renderer() {
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
}

void Renderer::RenderScene(const Scene& scene, const Camera& camera, const TransformComponent& transform) {
	framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	framebuffer.Unbind();
}