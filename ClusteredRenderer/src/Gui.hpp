#pragma once

#include <Core.hpp>

#include "Renderer/Window.hpp"

#include "Gui/ContentBrowserWindow.hpp"
#include "Gui/InspectorWindow.hpp"
#include "Gui/SceneWindow.hpp"
#include "Gui/ViewportWindow.hpp"

#include "Gui/MenuBar.hpp"

class Gui {
public:
	Gui(const Window& window);
	~Gui();

	void NewFrame() const;
	void Render(const Window& window) const;
};