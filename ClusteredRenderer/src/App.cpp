#include "App.hpp"

#include "Renderer/Window.hpp"
#include "Gui.hpp"

int App::Run() {
	
	Window window(640, 480, config::windowTitle, config::openGLVersion);

	Input::Init(window.glfwWindow());

	Gui gui(window);

	// Demo State
	bool show_demo_window = true;

	WindowTest1 windowTest1{ ImVec4(0.45f, 0.55f, 0.60f, 1.00f) };
	WindowTest2 windowTest2{};

	while (window.ShouldClose() == false) {
		Time::UpdateTime(glfwGetTime());

		// Get Inputs
		Input::ClearKeys();
		glfwPollEvents();

		if (Input::GetKeyDown(KB_G)) {
			spdlog::info("Current time: {}", Time::DeltaTime());
		}
		if (Input::GetKeyUp(KB_G)) {
			spdlog::info("Current FPS: {}", Time::FramesPerSecond());
		}

		// Prepare GUI
		gui.NewFrame();

		windowTest1.Draw(show_demo_window, windowTest2.GetDrawHandle());
		windowTest2.Draw();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// Clear Screen
		auto clearColor = windowTest1.GetClearColor();
		glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render
		// Scene.Render or Scene.Logic ...
		gui.Render(window);

		// Show frame
		window.SwapBuffers();
	}

	return EXIT_SUCCESS;
}