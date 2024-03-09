#pragma once

#include <Core.hpp>

struct ShaderRenderInfo{
	uint32 programId = 0;

	~ShaderRenderInfo() noexcept {
		Destroy();
	}

	ShaderRenderInfo(const ShaderRenderInfo&) = delete;
	ShaderRenderInfo& operator=(const ShaderRenderInfo&) = delete;

	ShaderRenderInfo(ShaderRenderInfo&& other) {
		programId = other.programId;
		other.programId = 0;
	}
	ShaderRenderInfo& operator=(ShaderRenderInfo&& other) {
		Destroy();
		programId = other.programId;
		other.programId = 0;
	}

private:
	void Destroy() {
		if (programId != 0) {
			glDeleteProgram(programId);
			programId = 0;
		}
	}
};