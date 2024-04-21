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
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
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
#include <imgui/implot.h>

#include <stb_image.h>

#include <magic_enum.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include <cereal_optional_nvp.h>

#include <tinyfiledialogs/tinyfiledialogs.h>

// Icons
#include <Gui/Definitions/IconsFontAwesome6.h>