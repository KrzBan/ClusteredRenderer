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

	ShaderRenderInfo(ShaderRenderInfo&& other) noexcept {
		programId = other.programId;
		other.programId = 0;
	}
	ShaderRenderInfo& operator=(ShaderRenderInfo&& other) noexcept {
		Destroy();
		programId = other.programId;
		other.programId = 0;
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