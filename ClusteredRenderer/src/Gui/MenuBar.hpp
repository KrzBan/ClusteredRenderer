#pragma once

#include <Core.hpp>
#include <SceneSerializer.hpp>

#include "Windows/GuiWindow.hpp"

class MenuBar {
	
public:
	void Draw(const std::vector<GuiWindow*>& windows, Scene& scene) {
		ImGui::BeginMainMenuBar();

		static const char* filterPatterns[] = { "*.scene" };

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save Scene")) {
				const auto path = tinyfd_saveFileDialog("Save Scene", ASSETS_DIR_L "\\assets\\Scene.scene", 1, filterPatterns, "Scene files (.scene)");
				if (path != nullptr) {
					SceneSerializer::SaveScene(scene, path);
				}
			}
			if (ImGui::MenuItem("Load Scene")) {
				const auto path = tinyfd_openFileDialog("Load Scene", ASSETS_DIR_L "\\assets\\Scene.scene", 1, filterPatterns, "Scene files (.scene)", 0);
				if (path != nullptr) {
					scene = SceneSerializer::LoadScene(path);
				}
			}
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