#pragma once

#include <Core.hpp>
#include <Scene.hpp>

struct SceneWindowOutput {
	
};

class SceneWindow {
private:
	bool m_DrawThis = true;

public:
	SceneWindow() {}

	bool& GetDrawHandle() { return m_DrawThis; }

	SceneWindowOutput Draw(const Scene& scene) {
		SceneWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin("Scene", &m_DrawThis)) {
			
		}
		ImGui::End();

		return output;
	}
};