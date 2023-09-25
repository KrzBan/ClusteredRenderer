#include "App.hpp"

#include "Renderer/Window.hpp"
#include "Gui.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/EditorCamera.hpp"

int App::Run() {
	
	Window window(1920, 1080, config::windowTitle, config::openGLVersion);

	Input::Init(window.glfwWindow());

	Scene scene{};
	EditorCamera editorCamera{45, 16/9, 0.1f, 1000.0f};

	Gui gui(window);
	InspectorWindow inspectorWindow{};
	SceneWindow sceneWindow{};
	ViewportWindow viewportWindow{};

	Framebuffer viewportFB{ 1, 1, 0 };

	bool runtime = false;

	while (window.ShouldClose() == false) {
		Time::UpdateTime(glfwGetTime());

		// Get Inputs
		Input::ClearKeys();
		glfwPollEvents();
		
		// Clear Screen	
		viewportFB.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Scripts & Render
		if (runtime) {
			scene.OnUpdateRuntime(Time::DeltaTime());
		} else {
			scene.OnUpdateEditor(Time::DeltaTime(), editorCamera);
		}

		viewportFB.Unbind();

		// Prepare GUI
		gui.NewFrame();

		// Draw Editor Windows
		auto inspectorWindowOutput = inspectorWindow.Draw();
		auto sceneWindowOutput = sceneWindow.Draw(scene);
		auto viewportWindowOutput = viewportWindow.Draw(viewportFB.GetColorAttachmentTextureID());

		gui.Render(window);

		// Update cameras based on viewport dimensions
		viewportFB.Resize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
		editorCamera.SetViewportSize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
		if (runtime) {
			scene.OnViewportResize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
		}

		// Show frame
		window.SwapBuffers();
	}

	return EXIT_SUCCESS;
}