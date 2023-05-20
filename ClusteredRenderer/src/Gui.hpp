#pragma once

#include <Core.hpp>

#include "Window.hpp"

#include "Gui/WindowTest1.hpp"
#include "Gui/WindowTest2.hpp"

class Gui {
public:
	Gui(const Window& window);
	~Gui();

	void NewFrame() const;
	void Render(const Window& window) const;
};