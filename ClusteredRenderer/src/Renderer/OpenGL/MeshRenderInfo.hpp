#pragma once

#include <Core.hpp>

struct MeshRenderInfo{
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	MeshRenderInfo() = default;

	~MeshRenderInfo() noexcept {
		Destroy();
	}

	MeshRenderInfo(const MeshRenderInfo&) = delete;
	MeshRenderInfo& operator=(const MeshRenderInfo&) = delete;

	void TakeOwnership(MeshRenderInfo&& other) {
		vao = other.vao;
		vbo = other.vbo;
		ebo = other.ebo;
		other.vao = 0;
		other.vbo = 0;
		other.ebo = 0;
	}

	MeshRenderInfo(MeshRenderInfo&& other) noexcept {
		TakeOwnership(std::move(other));
	}
	MeshRenderInfo& operator=(MeshRenderInfo&& other) noexcept {
		Destroy();
		TakeOwnership(std::move(other));
		return *this;
	}

private:
	void Destroy() {
		if (vao != 0) {
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
		if (vbo != 0) {
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}
		if (ebo != 0) {
			glDeleteBuffers(1, &ebo);
			ebo = 0;
		}
	}
};