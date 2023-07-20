#pragma once

#include <Core.hpp>

struct WindowTest1Output {
	std::optional<ImVec4> clearColor;
};

class WindowTest1 {
private:
	bool m_DrawThis = true;
	ImVec4 m_ClearColor;

public:
	WindowTest1(const ImVec4 clearColor ) : m_ClearColor{ clearColor }{}

	ImVec4 GetClearColor() const { return m_ClearColor; }
	bool& GetDrawHandle() { return m_DrawThis; }

	WindowTest1Output Draw(bool& showDemoWindow, bool& showWindowTest2) {
		WindowTest1Output output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin("Hello, world!")) {
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemoWindow);		// Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &showWindowTest2);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);				
			if (ImGui::ColorEdit3("clear color", (float*)&m_ClearColor)) {
				output.clearColor = m_ClearColor;
			}

			if (ImGui::Button("Button"))								// Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			auto& io = ImGui::GetIO();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		}
		ImGui::End();

		return output;
	}
};