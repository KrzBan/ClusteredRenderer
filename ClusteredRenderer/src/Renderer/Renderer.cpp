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