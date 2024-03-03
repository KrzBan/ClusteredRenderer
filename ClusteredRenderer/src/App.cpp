#include "App.hpp"

#include <Core/Input.hpp>
#include <Core/Time.hpp>

#include "Assets/AssetManager.hpp"
#include "Assets/AssetWatcher.hpp"

#include "Scene.hpp"

#include "Renderer/Renderer.hpp"
#include "Renderer/Window.hpp"
#include "Gui.hpp"

#include "Renderer/EditorCamera.hpp"

int App::Run() {
	
	spdlog::set_level(spdlog::level::trace);
	Window window(1920, 1080, config::windowTitle, config::openGLVersion);

	Input::Init(window.glfwWindow());

	Scene scene{};
	EditorCamera editorCamera{45, 16/9, 0.1f, 1000.0f};
	TransformComponent editorCameraTransform{};

	AssetManager::Init(ASSETS_DIR);

	// File Watcher
	const auto listener = std::make_unique<UpdateListener>();
	efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();
	efsw::WatchID watchID = fileWatcher->addWatch(ASSETS_DIR, listener.get(), true);
	fileWatcher->watch();

	Gui gui(window);
	AssetManagerWindow assetManagerWindow{};
	ContentBrowserWindow contentBrowserWindow{};
	InspectorWindow inspectorWindow{};
	SceneWindow sceneWindow{};
	ViewportWindow viewportWindow{};
	const std::vector<GuiWindow*> pWindows = { &assetManagerWindow, &contentBrowserWindow,
		&inspectorWindow, &sceneWindow, &viewportWindow };

	MenuBar menuBar;

	Renderer renderer{};

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
		
		// Render
		if (runtime) {
			auto cameraEntity = scene.GetPrimaryCameraEntity();
			renderer.RenderScene(scene, cameraEntity.GetComponent<CameraComponent>().Camera, cameraEntity.GetComponent<TransformComponent>());
			//scene.RenderSceneRuntime();
		}
		else {
			renderer.RenderScene(scene, editorCamera, editorCameraTransform);
			//scene.RenderSceneEditor(editorCamera);
		}

		// Prepare GUI
		gui.NewFrame(); 

		// Draw Editor Windows
		menuBar.Draw(pWindows);
		auto contentBrowserWindowOutput = contentBrowserWindow.Draw();
		if (contentBrowserWindowOutput.selectionChanged)
			sceneWindow.ResetSelection();
		auto sceneWindowOutput = sceneWindow.Draw(scene);
		if (sceneWindowOutput.selectionChanged)
			contentBrowserWindow.ResetSelection();

		std::variant<std::monostate, Entity, std::filesystem::path> selection;
		if (contentBrowserWindowOutput.selectedFile != "") {
			selection = contentBrowserWindowOutput.selectedFile;
		}
		if (sceneWindowOutput.selectedEntity) {
			selection = sceneWindowOutput.selectedEntity;
		} 

		auto inspectorWindowOutput = inspectorWindow.Draw(selection);
		auto viewportWindowOutput = viewportWindow.Draw(renderer.framebuffer.GetColorAttachmentTextureID());
		auto assetManagerWindowOutput = assetManagerWindow.Draw();
		gui.Render(window);

		// Update cameras based on viewport dimensions
		if (viewportWindowOutput.windowWidth != 0 && viewportWindowOutput.windowHeight != 0) {
			renderer.framebuffer.Resize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			editorCamera.SetViewportSize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			if (runtime) {
				scene.OnViewportResize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			}
		}

		// Update asset changes
		AssetManager::HandleFileChanges(listener->FlushQueue());

		// Show frame
		window.SwapBuffers();
	}

	AssetManager::Clear();

	return EXIT_SUCCESS;
}