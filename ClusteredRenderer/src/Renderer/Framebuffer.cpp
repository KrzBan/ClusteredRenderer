#include "Framebuffer.hpp"

Framebuffer::Framebuffer(uint32 width, uint32 height, bool hdr)
	: m_Width{ width }, m_Height{ height }, m_Hdr{ hdr } {
	Construct();
}
Framebuffer::~Framebuffer() noexcept {
	Destroy();
}

void Framebuffer::Resize(uint32 width, uint32 height) {
	if (m_Width == width && m_Height == height) return;

	m_Width = width;
	m_Height = height;

	Destroy();
	Construct();
}

void Framebuffer::Construct() {
	if (m_Width == 0 || m_Height == 0) return;

	glGenFramebuffers(1, &m_FboID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);

	glGenTextures(1, &m_ColTexID);

	glBindTexture(GL_TEXTURE_2D, m_ColTexID);
	if (not m_Hdr) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColTexID, 0);
	

	glGenRenderbuffers(1, &m_RboID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RboID);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RboID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Destroy() {
	if (m_FboID != 0) glDeleteFramebuffers(1, &m_FboID);
	if (m_ColTexID != 0) glDeleteTextures(1, &m_ColTexID);

	m_FboID = 0;
	m_ColTexID = 0;
}

bool Framebuffer::Bind(uint32 target) const {
	if (m_FboID == 0) return false;
	glBindFramebuffer(target, m_FboID);
	glViewport(0, 0, m_Width, m_Height);

	return true;
}

void Framebuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32 Framebuffer::GetWidth() const {
	return m_Width;
}
uint32 Framebuffer::GetHeight() const {
	return m_Height;
}

uint32 Framebuffer::GetColorAttachmentTextureID() const {
	return m_ColTexID;
}
uint32 Framebuffer::GetDepthAttachmentTextureID() const {
	return m_FboID;
}