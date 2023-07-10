#include "Input.hpp"

struct KeyState {
	bool isPressedDown = false;	//one frame down
	bool isPressed = false;		//just down
	bool released = false;		//one frame released
};


struct InputData {
	std::array<KeyState, GLFW_KEY_LAST> keyStates{};
	std::vector<int> releasedKeys;

	double MousePosX = 0.0, MousePosY = 0.0;
	double MouseDeltaX = 0.0, MouseDeltaY = 0.0;
};

static InputData inputData{};

void Input::Init(GLFWwindow* window) {
	glfwSetKeyCallback(window, Input::InputCallback);
	glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
	glfwSetCursorPosCallback(window, Input::MousePosCallback);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	inputData.releasedKeys.reserve(GLFW_KEY_LAST + 1);
}

bool Input::GetKey(int keyCode) { return inputData.keyStates[keyCode].isPressed; }
bool Input::GetKeyDown(int keyCode) { return inputData.keyStates[keyCode].isPressedDown; }
bool Input::GetKeyUp(int keyCode) { return inputData.keyStates[keyCode].released; }

double Input::GetMousePosX() { return inputData.MousePosX; }
double Input::GetMousePosY() { return inputData.MousePosY; }

double Input::GetMouseDeltaX() { return  inputData.MouseDeltaX; }
double Input::GetMouseDeltaY() { return  inputData.MouseDeltaY; }

void Input::ClearKeys() {
	inputData.MouseDeltaX = 0;
	inputData.MouseDeltaY = 0;
	for (int key : inputData.releasedKeys) {
		inputData.keyStates[key].released = false;
		inputData.keyStates[key].isPressedDown = false;
	}
	inputData.releasedKeys.clear();
}

void Input::InputCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {

	switch (action) {
	case GLFW_PRESS:
		if (inputData.keyStates[key].isPressed == false)
			inputData.keyStates[key].isPressedDown = true;
		inputData.keyStates[key].isPressed = true;
		inputData.releasedKeys.push_back(key);
		break;
	case GLFW_RELEASE:
		inputData.keyStates[key].isPressedDown = false;;
		inputData.keyStates[key].isPressed = false;
		inputData.keyStates[key].released = true;
		inputData.releasedKeys.push_back(key);
		break;
	}
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	InputCallback(window, button, 0, action, mods);
}

void Input::MousePosCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
	inputData.MousePosX = xpos;
	inputData.MousePosY = ypos;
}