#include "App.hpp"

#include <Core/Input.hpp>
#include <Core/Time.hpp>

#include "Scene.hpp"

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
	ContentBrowserWindow contentBrowserWindow{};
	InspectorWindow inspectorWindow{};
	SceneWindow sceneWindow{};
	ViewportWindow viewportWindow{};

	Framebuffer viewportFB{ 1, 1, 0 };

	bool runtime = false;
	Timestep physicsAccumulator{ 0.0 };

	while (window.ShouldClose() == false) {
		Time::UpdateTime(glfwGetTime());

		// Get Inputs
		Input::ClearKeys();
		glfwPollEvents();
		
		physicsAccumulator += Time::DeltaTime();
		while (physicsAccumulator >= Time::FixedDeltaTime()) {
			scene.OnUpdateSimulation(Time::FixedDeltaTime());
			physicsAccumulator -= Time::FixedDeltaTime();
		}

		// Update Logic
		if (runtime) {
			scene.OnUpdateRuntime(Time::DeltaTime());
		} else {
			scene.OnUpdateEditor(Time::DeltaTime());
		}

		// Clear Screen
		viewportFB.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render
		if (runtime) {
			scene.RenderSceneRuntime();
		}
		else {
			scene.RenderSceneEditor(editorCamera);
		}

		viewportFB.Unbind();

		// Prepare GUI
		gui.NewFrame();

		// Draw Editor Windows
		auto contentBrowserWindowOutput = contentBrowserWindow.Draw();
		auto sceneWindowOutput = sceneWindow.Draw(scene);
		auto inspectorWindowOutput = inspectorWindow.Draw(sceneWindowOutput.selectedEntity);
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