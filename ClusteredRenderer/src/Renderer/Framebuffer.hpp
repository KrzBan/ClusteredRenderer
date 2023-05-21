#pragma once

#include <Core.hpp>

class Framebuffer {
public:
	Framebuffer(uint32 width, uint32 height, uint32 samples);
	~Framebuffer();

	void Resize(uint32 width, uint32 height);

	bool Bind() const;
	bool Bind(uint32 target) const;
	void Unbind() const;

	uint32 GetWidth() const;
	uint32 GetHeight() const;

	uint32 GetColorAttachmentTextureID() const;
	uint32 GetDepthAttachmentTextureID() const;

private:
	void Construct();
	void Destroy();
private:
	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_Samples = 0;

	uint32 m_FboID = 0;
	uint32 m_ColTexID = 0;
	uint32 m_RboID = 0;
};