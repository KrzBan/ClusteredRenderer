#pragma once

#include "Core.hpp"

class Window {

public:
	Window(int width, int height, const char* title, OpenGLVersion openGLVersion);
	~Window();

	GLFWwindow* glfwWindow() const;
	bool ShouldClose() const;
	void SwapBuffers() const;

	static void ErrorCallback(int error, const char* description);

private:
	GLFWwindow* m_Window = nullptr;
};