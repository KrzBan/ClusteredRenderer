add_library(ClusteredRendererLib)

set_property(TARGET ClusteredRendererLib PROPERTY CXX_STANDARD 20)

target_sources(ClusteredRendererLib PUBLIC 
	"Core.hpp" "Core.cpp" "App.hpp" "App.cpp" 
	"Core/Time.hpp" "Core/Time.cpp" "Core/Input.hpp" "Core/Input.cpp" 
	"Core/KeyCodes.hpp"  "Core/KeyCodes.cpp"
	"Gui.hpp" "Gui.cpp"
	"Scene.hpp" "Scene.cpp" 
	"Stb.cpp"
	"Renderer/Framebuffer.hpp" "Renderer/Framebuffer.cpp" 
	"Renderer/Shader.hpp" "Renderer/Shader.cpp" 
	"Renderer/ShaderProgram.hpp" "Renderer/ShaderProgram.cpp" 
	"Renderer/Texture.hpp" "Renderer/Texture.cpp"
	"Renderer/Window.hpp" "Renderer/Window.cpp"   )

find_package(spdlog)
find_package(glfw3)
find_package(assimp)
find_package(EnTT)
find_package(glm)
find_package(Stb REQUIRED)
    
target_link_libraries(ClusteredRendererLib PUBLIC imguiLib gladLib spdlog::spdlog glfw assimp::assimp EnTT::EnTT glm::glm)

target_include_directories(ClusteredRendererLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(ClusteredRendererLib PRIVATE ${Stb_INCLUDE_DIR})