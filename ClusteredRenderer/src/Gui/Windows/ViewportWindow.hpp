#pragma once

#include <Core.hpp>

#include "GuiWindow.hpp"

struct ViewportWindowOutput {
	uint32 windowWidth{}, windowHeight{};
	bool isWindowFocused{};
};

class ViewportWindow : public GuiWindow {
#define VIEWPORT_NAME "Viewport"
public:
	ViewportWindow() {}

	virtual const char* GetName() const override {
		return VIEWPORT_NAME;
	};

	ViewportWindowOutput Draw(uint32 textureId) {
		ViewportWindowOutput output{};

		if (m_DrawThis == false) return output;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

		if (ImGui::Begin(ICON_FA_GAMEPAD " " VIEWPORT_NAME, &m_DrawThis)) {
			ImGui::BeginChild("ViewportRender");

			output.isWindowFocused = ImGui::IsWindowFocused();

			ImVec2 windowSize = ImGui::GetWindowSize();
			output.windowWidth = static_cast<uint32>(windowSize.x);
			output.windowHeight = static_cast<uint32>(windowSize.y);

			ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(textureId)), windowSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();

		ImGui::PopStyleVar();

		return output;
	}
};