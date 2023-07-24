#include "App.hpp"

#include "Renderer/Window.hpp"
#include "Gui.hpp"

int App::Run() {
	
	Window window(640, 480, config::windowTitle, config::openGLVersion);

	Input::Init(window.glfwWindow());

	Gui gui(window);
	ViewportWindow viewport{};

	while (window.ShouldClose() == false) {
		Time::UpdateTime(glfwGetTime());

		// Get Inputs
		Input::ClearKeys();
		glfwPollEvents();

		// Scripts
		//scene.OnUpdate(Input::DeltaTime());
		
		// Prepare GUI
		gui.NewFrame();

		// Draw Editor Windows
		auto viewportOutput = viewport.Draw(0);
		// update Cameras
		// update Viewport
		
		// Clear Screen	
		glClear(GL_COLOR_BUFFER_BIT);

		// Render
		// Scene.Render or Scene.Logic ...
		gui.Render(window);

		// Show frame
		window.SwapBuffers();
	}

	return EXIT_SUCCESS;
}