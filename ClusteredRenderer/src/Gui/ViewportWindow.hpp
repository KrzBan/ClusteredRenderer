#pragma once

#include <Core.hpp>

struct ViewportWindowOutput {
	uint32 windowWidth{}, windowHeight{};
};

class ViewportWindow {
private:
	bool m_DrawThis = true;

public:
	ViewportWindow() {}

	bool& GetDrawHandle() { return m_DrawThis; }

	ViewportWindowOutput Draw(uint32 textureId) {
		ViewportWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_GAMEPAD " Viewport", &m_DrawThis)) {
			ImGui::BeginChild("ViewportRender");

			ImVec2 windowSize = ImGui::GetWindowSize();
			output.windowWidth = static_cast<uint32>(windowSize.x);
			output.windowHeight = static_cast<uint32>(windowSize.y);

			ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(textureId)), windowSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();

		return output;
	}
};