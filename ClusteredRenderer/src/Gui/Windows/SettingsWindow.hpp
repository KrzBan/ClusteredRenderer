#pragma once

#include <Core.hpp>
#include <Core/Time.hpp>

#include "GuiWindow.hpp"

struct SettingsWindowOutput {

};

class SettingsWindow : public GuiWindow {
#define SETTINGS_NAME "Settings"

public:
	SettingsWindow() {}

	virtual const char* GetName() const override {
		return SETTINGS_NAME;
	};

	SettingsWindowOutput Draw(std::chrono::nanoseconds renderTimeNs) {
		SettingsWindowOutput output{};
		
		using namespace std::chrono_literals;

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_GEAR " " SETTINGS_NAME, &m_DrawThis)) {
			if (ImGui::BeginTable("Settings", 2)) {
				ImGui::TableNextColumn();
				ImGui::Text("Render Time");
				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f} ms", renderTimeNs / 1.0ms).c_str());

				ImGui::TableNextColumn();
				ImGui::Text("Frame Time");
				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f} ms", Time::DeltaTime()*1000.0f).c_str());

				ImGui::EndTable();
			}
		}
		ImGui::End();

		return output;
	}
};