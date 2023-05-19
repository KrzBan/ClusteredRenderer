project(imguiLib)

add_library(imguiLib SHARED
    "imgui_impl_glfw.h" "imgui_impl_glfw.cpp" 
	"imgui_impl_opengl3.h" "imgui_impl_opengl3.cpp" "imgui_impl_win32.h" "imgui_impl_win32.cpp"
	"imgui.h" "imgui.cpp" "imgui_internal.h" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "imgui_demo.cpp"
	"imstb_rectpack.h" "imstb_textedit.h" "imstb_truetype.h")


find_package(glfw3)
target_link_libraries(imguiLib glfw)

target_include_directories(imguiLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
