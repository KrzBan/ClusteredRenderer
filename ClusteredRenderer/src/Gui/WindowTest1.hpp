#pragma once

#include <Core.hpp>

class WindowTest1 {
private:
	bool m_DrawThis = true;
	ImVec4 m_ClearColor;

public:
	WindowTest1(const ImVec4 clearColor ) : m_ClearColor{ clearColor }{}

	ImVec4 GetClearColor() const { return m_ClearColor; }
	bool& GetDrawHandle() { return m_DrawThis; }

	void Draw(bool& showDemoWindow, bool& showWindowTest2) {
		if (m_DrawThis == false) return;

		if (ImGui::Begin("Hello, world!")) {
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemoWindow);		// Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &showWindowTest2);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);				// Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&m_ClearColor);	// Edit 3 floats representing a color

			if (ImGui::Button("Button"))								// Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			auto& io = ImGui::GetIO();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		}
		ImGui::End();
	}
};