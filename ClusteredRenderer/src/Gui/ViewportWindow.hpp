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

		if (ImGui::Begin("Viewport", &m_DrawThis)) {
			ImGui::BeginChild("ViewportRender");

			ImVec2 windowSize = ImGui::GetWindowSize();
			output.windowWidth = windowSize.x;
			output.windowHeight = windowSize.y;

			ImGui::Image((ImTextureID)textureId, windowSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();

		return output;
	}
};