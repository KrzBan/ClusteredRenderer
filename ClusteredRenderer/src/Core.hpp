#pragma once

#include <CoreImports.hpp>

#include "CerealCustomTypes.hpp"

struct OpenGLVersion {
	int major = 0;
	int minor = 0;
};

#define ASSETS_DIR "F:/dev/ClusteredRendererWorkspace"
#define ASSETS_DIR_L "F:\\dev\\ClusteredRendererWorkspace"
#define RESOURCES_DIR "./resources/"

namespace config {
	constexpr char const* windowTitle = "Program";
	constexpr char const* glslVersion = "#version 430";
	constexpr OpenGLVersion openGLVersion{ 4, 3 };

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