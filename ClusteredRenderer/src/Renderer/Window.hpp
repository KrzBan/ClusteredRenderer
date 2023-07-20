#pragma once

#include "Core.hpp"

class Window {

public:
	Window(int width, int height, const char* title, OpenGLVersion openGLVersion);
	~Window();

	GLFWwindow* glfwWindow() const;

	uint32 GetWidth() const;
	uint32 GetHeight() const;

	bool ShouldClose() const;
	void SetShouldClose(bool state) const;

	void PollEvents() const;
	void SwapBuffers() const;

	static void ErrorCallback(int error, const char* description);

	void SetTitle(const std::string& title);
	void SetClearColor(const glm::vec4& clearColor);

private:
	GLFWwindow* m_Window = nullptr;

	uint32 m_Width = 0;
	uint32 m_Height = 0;
	
};