#pragma once

#include <Core.hpp>

#include "Renderer/Window.hpp"

#include "Gui/InspectorWindow.hpp"
#include "Gui/SceneWindow.hpp"
#include "Gui/ViewportWindow.hpp"

class Gui {
public:
	Gui(const Window& window);
	~Gui();

	void NewFrame() const;
	void Render(const Window& window) const;
};