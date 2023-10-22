#pragma once

#include <Core.hpp>

#include "Windows/GuiWindow.hpp"

class MenuBar {
	
public:
	void Draw(const std::vector<GuiWindow*>& windows) {
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New Scene");
			ImGui::MenuItem("Load Scene");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {
			if (ImGui::BeginMenu("Windows")) {

				for (auto& window : windows) {
					auto& draw = window->GetDrawHandle();
					const auto name = window->GetName();
					const auto label = draw
										   ? std::format(ICON_FA_CHECK "{0}###{0}", name)
										   : std::format("{0}###{0}", name);
					if (ImGui::MenuItem(label.c_str())) {
						draw = not draw;
					}
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

};