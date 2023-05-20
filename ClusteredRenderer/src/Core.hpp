#pragma once

#include <iostream>
#include <algorithm>
#include <array>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <Core/Input.hpp>
#include <Core/Time.hpp>

struct OpenGLVersion {
	int major = 0;
	int minor = 0;
};

namespace config {
	constexpr char const* windowTitle = "Program";
	constexpr char const* glslVersion = "#version 400";
	constexpr OpenGLVersion openGLVersion{ 4, 0 };
}

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Shared = std::unique_ptr<T>;
