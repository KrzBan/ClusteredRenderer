#pragma once

#include <algorithm>
#include <array>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <entt/entt.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

#define IM_VEC2_CLASS_EXTRA \
    constexpr ImVec2(glm::vec2& f) : x(f.x), y(f.y) {} \
    operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA \
        constexpr ImVec4(const glm::vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
        operator glm::vec4() const { return glm::vec4(x,y,z,w); }

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include <stb_image.h>

// Icons
#include <Gui/Definitions/IconsFontAwesome6.h>

struct OpenGLVersion {
	int major = 0;
	int minor = 0;
};

#define ASSETS_DIR "F:/dev/ClusteredRendererWorkspace"
#define RESOURCES_DIR "./resources/"

namespace config {
	constexpr char const* windowTitle = "Program";
	constexpr char const* glslVersion = "#version 400";
	constexpr OpenGLVersion openGLVersion{ 4, 0 };

	constexpr char const* assetDirectory = ASSETS_DIR;
}

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

using uint32 = uint32_t;
using uint64 = uint64_t;

#define ColorFromInt(r,g,b,a) r/255.0f, g/255.0f, b/255.0f, a/255.0f 

template <class... Ts>
struct overload : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

template <>
struct fmt::formatter<std::filesystem::path> : formatter<std::string_view> {
	template <typename FormatContext>
	auto format(const std::filesystem::path& path, FormatContext& ctx) {
		return formatter<std::string_view>::format(path.string(), ctx);
	}
};
