#pragma once

#include <Core.hpp>

struct Texture2DRenderInfo{
	GLuint textureId = 0;

	Texture2DRenderInfo() = default;

	~Texture2DRenderInfo() noexcept {
		Destroy();
	}

	Texture2DRenderInfo(const Texture2DRenderInfo&) = delete;
	Texture2DRenderInfo& operator=(const Texture2DRenderInfo&) = delete;

	void TakeOwnership(Texture2DRenderInfo&& other) {
		textureId = other.textureId;
		other.textureId = 0;
	}

	Texture2DRenderInfo(Texture2DRenderInfo&& other) noexcept {
		TakeOwnership(std::move(other));
	}
	Texture2DRenderInfo& operator=(Texture2DRenderInfo&& other) noexcept {
		Destroy();
		TakeOwnership(std::move(other));
		return *this;
	}

private:
	void Destroy() {
		if (textureId != 0) {
			glDeleteTextures(1, &textureId);
			textureId = 0;
		}
	}
};