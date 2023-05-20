#pragma once

#include <Core.hpp>

#include "KeyCodes.hpp"

struct Input {
	static void Init(GLFWwindow* window);

	static bool GetKey(int keyCode);
	static bool GetKeyDown(int keyCode);
	static bool GetKeyUp(int keyCode);

	static double GetMousePosX();
	static double GetMousePosY();

	static double GetMouseDeltaX();
	static double GetMouseDeltaY();

	static void InputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void ClearKeys();
};