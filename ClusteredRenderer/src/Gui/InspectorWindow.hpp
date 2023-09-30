#pragma once

#include <Core.hpp>

struct InspectorWindowOutput {
	
};

class InspectorWindow {
private:
	bool m_DrawThis = true;

public:
	InspectorWindow() {}

	bool& GetDrawHandle() { return m_DrawThis; }

	InspectorWindowOutput Draw() {
		InspectorWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_NEWSPAPER " Inspector", &m_DrawThis)) {
			
		}
		ImGui::End();

		return output;
	}
};