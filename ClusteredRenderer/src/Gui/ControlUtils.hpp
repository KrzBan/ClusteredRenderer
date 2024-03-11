#pragma once

#include <Core.hpp>

class ControlUtils {
public:
	static void DrawFloat(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec2(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec4(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columnWidth = 100.0f);
};