#pragma once

#include <Core.hpp>

class WindowTest2 {
private:
	bool m_DrawThis = true;

public:
	WindowTest2() = default;

	bool& GetDrawHandle() { return m_DrawThis; }

	void Draw() {
		if (m_DrawThis == false) return;

		if (ImGui::Begin("Another Window", &m_DrawThis)) {
			
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				m_DrawThis = false;
			
		}
		ImGui::End();
	}
};