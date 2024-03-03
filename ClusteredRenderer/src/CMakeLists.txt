add_library(ClusteredRendererLib)

set_property(TARGET ClusteredRendererLib PROPERTY CXX_STANDARD 20)

target_sources(ClusteredRendererLib PUBLIC 
	"App.hpp" "App.cpp" 

	"Assets/AssetManager.hpp" "Assets/AssetManager.cpp"
	"Assets/AssetWatcher.hpp" "Assets/AssetWatcher.cpp"
	"Assets/FileUtils.hpp"
	"Assets/CommonMetaData.hpp" "Assets/CommonMetaData.cpp"

	"Assets/AssetTypes/AssetType.hpp"
	"Assets/AssetTypes/MaterialAsset.hpp"
	"Assets/AssetTypes/TextAsset.hpp"

	"cereal_optional_nvp.h"

	"Components.hpp"

	"Core.hpp" "Core.cpp" 
	"Core/Input.hpp" "Core/Input.cpp" 
	"Core/KeyCodes.hpp" "Core/KeyCodes.cpp"
	"Core/Time.hpp" "Core/Time.cpp"

	"Entity.hpp" "Entity.cpp"
	
	"Gui.hpp" "Gui.cpp"
	"Gui/ControlUtils.hpp" "Gui/ControlUtils.cpp"

	"Gui/Definitions/IconsFontAwesome6.h"

	"Gui/MenuBar.hpp"

	"Gui/Windows/AssetManagerWindow.hpp"
	"Gui/Windows/ContentBrowserWindow.hpp"
	"Gui/Windows/GuiWindow.hpp"
	"Gui/Windows/InspectorWindow.hpp"
	"Gui/Windows/SceneWindow.hpp"
	"Gui/Windows/ViewportWindow.hpp"

	"Scene.hpp" "Scene.cpp" 
	"SceneCamera.hpp" "SceneCamera.cpp"
	"Scriptable.hpp"
	"Stb.cpp"

	"Renderer/Camera.hpp"
	"Renderer/EditorCamera.hpp" "Renderer/EditorCamera.cpp"
	"Renderer/Framebuffer.hpp" "Renderer/Framebuffer.cpp" 
	"Renderer/Renderer.hpp" "Renderer/Renderer.cpp"
	"Renderer/Shader.hpp" "Renderer/Shader.cpp" 
	"Renderer/ShaderProgram.hpp" "Renderer/ShaderProgram.cpp" 
	"Renderer/Texture.hpp" "Renderer/Texture.cpp"
	"Renderer/Window.hpp" "Renderer/Window.cpp"    
	
	"UUID.hpp" "UUID.cpp")

find_package(spdlog REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(Stb REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(efsw CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

target_link_libraries(ClusteredRendererLib PUBLIC imguiLib gladLib
	spdlog::spdlog glfw assimp::assimp EnTT::EnTT glm::glm cereal::cereal efsw::efsw magic_enum::magic_enum)

# Supress warnings - Spdlog does not set it's includes as SYSTEM
get_target_property(_inc spdlog::spdlog INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(spdlog::spdlog SYSTEM INTERFACE ${_inc})

target_include_directories(ClusteredRendererLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(ClusteredRendererLib PRIVATE ${Stb_INCLUDE_DIR})