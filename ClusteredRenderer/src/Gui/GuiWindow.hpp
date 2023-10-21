#pragma once

#include <Core.hpp>

class GuiWindow {
protected:
	bool m_DrawThis = true;

public:
	bool& GetDrawHandle() { return m_DrawThis; }
	virtual const char* GetName() const = 0;
};