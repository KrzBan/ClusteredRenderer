#pragma once

#include <Core.hpp>

#include "Renderer/Window.hpp"

#include "Gui/Windows/AssetManagerWindow.hpp"
#include "Gui/Windows/ContentBrowserWindow.hpp"
#include "Gui/Windows/InspectorWindow.hpp"
#include "Gui/Windows/SettingsWindow.hpp"
#include "Gui/Windows/SceneWindow.hpp"
#include "Gui/Windows/ViewportWindow.hpp"

#include "Gui/MenuBar.hpp"

class Gui {
public:
	Gui(const Window& window);
	~Gui();

	void NewFrame() const;
	void Render(const Window& window) const;
};