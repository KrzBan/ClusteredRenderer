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

#include <Utils/AppState.hpp>

int App::Run() {
	spdlog::set_level(spdlog::level::trace);
	Window window(1920, 1080, config::windowTitle, config::openGLVersion);

	Input::Init(window.glfwWindow());

	Scene scene{};
	Scene sceneRevert{};

	EditorCamera editorCamera{45, 16/9, 0.1f, 1000.0f};

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
	SettingsWindow settingsWindow{};
	SceneWindow sceneWindow{};
	ViewportWindow viewportWindow{};
	const std::vector<GuiWindow*> pWindows = { &assetManagerWindow, &contentBrowserWindow,
		&inspectorWindow, &settingsWindow, &sceneWindow, &viewportWindow };
	MenuBar menuBar;

	Renderer renderer{};
	GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	AppState appState = AppStateEnum::EDITOR;
	Timestep physicsAccumulator{ 0.0 };
	std::chrono::nanoseconds renderTimeNs{};

	while (window.ShouldClose() == false) {
		Time::UpdateTime(glfwGetTime());

		// Get Inputs
		Input::ClearKeys();
		glfwPollEvents();
		
		if (appState.SwitchedToGameConsume()) {
			sceneRevert = scene;
			scene.OnSimulationStart();
			scene.OnRuntimeStart();
		}
		if (appState.SwitchedToEditorConsume()) {
			scene = sceneRevert;
			sceneRevert = {};
			scene.OnSimulationStop();
			scene.OnRuntimeStop();
		}

		// Update Logic
		if (appState.isGame()) {
			physicsAccumulator += Time::DeltaTime();
			while (physicsAccumulator >= Time::FixedDeltaTime()) {
				scene.OnUpdateSimulation(Time::FixedDeltaTime());
				physicsAccumulator -= Time::FixedDeltaTime();
			}

			scene.OnUpdateRuntime(Time::DeltaTime());
		} else {
			scene.OnUpdateEditor(Time::DeltaTime());
		}
		
		std::chrono::steady_clock::time_point renderTsBegin = std::chrono::steady_clock::now();

		// Render
		if (appState.isRuntime()) {
			auto cameraEntity = scene.GetPrimaryCameraEntity();
			if (cameraEntity.has_value()) {
				renderer.RenderScene(scene,
					cameraEntity.value().GetComponent<CameraComponent>().Camera,
					cameraEntity.value().GetComponent<TransformComponent>().GetTransform());
			}
			else {
				renderer.RenderScene(scene, editorCamera, editorCamera.GetViewMatrix());
			}
		}
		else {
			renderer.RenderScene(scene, editorCamera, editorCamera.GetViewMatrix());
		}

		// while (glClientWaitSync(fence, 0, 0) != GL_ALREADY_SIGNALED) {
		// 	// Draw call finished
		// }
		
		std::chrono::steady_clock::time_point renderTsEnd = std::chrono::steady_clock::now();
		renderTimeNs = renderTsEnd - renderTsBegin;

		// Prepare GUI
		gui.NewFrame(); 

		// Draw Editor Windows
		menuBar.Draw(pWindows, scene, appState);
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
		auto viewportWindowOutput = viewportWindow.Draw(renderer.postprocessFbo.GetColorAttachmentTextureID());
		auto assetManagerWindowOutput = assetManagerWindow.Draw();
		const auto settingsWindowOutput = settingsWindow.Draw(renderTimeNs, renderer);
		gui.Render(window);

		// Update cameras based on viewport dimensions
		if (viewportWindowOutput.windowWidth != 0 && viewportWindowOutput.windowHeight != 0) {
			renderer.hdrFbo.Resize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			renderer.postprocessFbo.Resize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			editorCamera.SetViewportSize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			if (appState.isRuntime()) {
				scene.OnViewportResize(viewportWindowOutput.windowWidth, viewportWindowOutput.windowHeight);
			}
		}

		if (viewportWindowOutput.isWindowFocused && appState.isEditor()) {
			editorCamera.OnUpdate(Time::DeltaTime());
		}

		// Show frame
		window.SwapBuffers();

		// Update asset changes
		AssetManager::HandleFileChanges(listener->FlushQueue());
	}

	AssetManager::SaveAll();
	AssetManager::Clear();

	return EXIT_SUCCESS;
}