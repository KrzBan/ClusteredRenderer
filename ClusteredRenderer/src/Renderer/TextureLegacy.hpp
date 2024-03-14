#pragma once

#include <Core.hpp>

class TextureLegacy {
private:
	uint32 m_Handle;
	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_BPP = 0;
	uint32 m_DataFormat = 0;
	uint32 m_InternalFormat = 0;

	std::string m_Path;
public:
	TextureLegacy(std::string_view filepath);
	~TextureLegacy();

	void Bind(uint32 slot = 0);
	void Unbind();

	uint32 GetHandle() const;

	int GetWidth() const;
	int GetHeight() const;
};