#pragma once

#include <Core.hpp>
#include <SceneSerializer.hpp>

#include "Windows/GuiWindow.hpp"

class MenuBar {
	
public:
	void Draw(const std::vector<GuiWindow*>& windows, Scene& scene) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = ImGui::GetFrameHeight();

		
		if (ImGui::BeginMainMenuBar()) {
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

		if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", NULL, ImGuiDir_Up, height, window_flags)) {
			if (ImGui::BeginMenuBar()) {
				ImGuiStyle& style = ImGui::GetStyle();
				float width = 0.0f;
				width += ImGui::CalcTextSize(ICON_FA_PLAY).x;
				width += style.ItemSpacing.x;
				width += ImGui::CalcTextSize(ICON_FA_PAUSE).x;
				width += style.ItemSpacing.x;
				width += ImGui::CalcTextSize(ICON_FA_STOP).x;

				float avail = ImGui::GetContentRegionAvail().x;
				float off = (avail - width)/2;
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

				ImGui::Button(ICON_FA_PLAY);
				ImGui::Button(ICON_FA_PAUSE);
				ImGui::Button(ICON_FA_STOP);
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

};