add_library(gladLib
	glad/glad.h
	glad/glad.c
	KHR/khrplatform.h)
target_include_directories(gladLib SYSTEM PUBLIC ./)

add_library(imguiLib STATIC 
	"imgui/imgui_impl_glfw.h" "imgui/imgui_impl_glfw.cpp" 
	"imgui/imgui_impl_opengl3.h" "imgui/imgui_impl_opengl3.cpp" "imgui/imgui_impl_win32.h" "imgui/imgui_impl_win32.cpp"
	"imgui/imgui.h" "imgui/imgui.cpp" "imgui/imgui_internal.h" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "imgui/imgui_demo.cpp"
	"imgui/imstb_rectpack.h" "imgui/imstb_textedit.h" "imgui/imstb_truetype.h"
	"imgui/imgui_stdlib.h" "imgui/imgui_stdlib.cpp"
	"imgui/implot.h" "imgui/implot.cpp" "imgui/implot_internal.h" "imgui/implot_items.cpp")

find_package(glfw3)
target_link_libraries(imguiLib PRIVATE gladLib glfw)

target_include_directories(imguiLib PUBLIC SYSTEM ./)
