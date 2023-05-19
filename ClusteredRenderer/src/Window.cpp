#include "Window.hpp"

Window::Window(int width, int height, const char* title, OpenGLVersion openGLVersion) {
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit()) {
		throw std::runtime_error("Couldn't init glfw");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::openGLVersion.major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::openGLVersion.minor);

	m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_Window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Couldn't create glfw window");
	}

	glfwMakeContextCurrent(m_Window);
	gladLoadGL();
	glfwSwapInterval(1);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

GLFWwindow* Window::glfwWindow() const {
	return m_Window;
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void Window::SwapBuffers() const {
	glfwSwapBuffers(m_Window);
}



void Window::ErrorCallback(int error, const char* description) {
	spdlog::error("Error: {}", description);
}
