#pragma once

#include <Core.hpp>

struct ShaderRenderInfo{
	GLuint programId = 0;

	ShaderRenderInfo() = default;

	~ShaderRenderInfo() noexcept {
		Destroy();
	}

	ShaderRenderInfo(const ShaderRenderInfo&) = delete;
	ShaderRenderInfo& operator=(const ShaderRenderInfo&) = delete;

	void TakeOwnership(ShaderRenderInfo&& other) {
		programId = other.programId;
		other.programId = 0;
	}

	ShaderRenderInfo(ShaderRenderInfo&& other) noexcept {
		TakeOwnership(std::move(other));
	}
	ShaderRenderInfo& operator=(ShaderRenderInfo&& other) noexcept {
		Destroy();
		TakeOwnership(std::move(other));
		return *this;
	}

private:
	void Destroy() {
		if (programId != 0) {
			glDeleteProgram(programId);
			programId = 0;
		}
	}
};