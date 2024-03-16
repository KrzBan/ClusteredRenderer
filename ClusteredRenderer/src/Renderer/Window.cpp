#include "Window.hpp"

static void PrintVersionInfo();

Window::Window(int width, int height, const char* title, OpenGLVersion openGLVersion)
    : m_Width{ static_cast<uint32>(width) }, m_Height{ static_cast<uint32>(height) } {
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit()) {
		throw std::runtime_error("Couldn't init glfw");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGLVersion.major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGLVersion.minor);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  

	m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_Window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Couldn't create glfw window");
	}

	glfwMakeContextCurrent(m_Window);
	gladLoadGL();
	glfwSwapInterval(1);

    PrintVersionInfo();
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

uint32 Window::GetWidth() const {
    return m_Width;
}
uint32 Window::GetHeight() const {
    return m_Height;
}

GLFWwindow* Window::glfwWindow() const {
	return m_Window;
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void Window::SetShouldClose(bool state) const {
	glfwSetWindowShouldClose(m_Window, state);
}

void Window::PollEvents() const {
	glfwPollEvents();
}
void Window::SwapBuffers() const {
	glfwSwapBuffers(m_Window);
}

void Window::SetTitle(const std::string& title) {
    glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::SetClearColor(const glm::vec4& clearColor) {
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
}


void Window::ErrorCallback( [[maybe_unused]] int error, const char* description) {
	spdlog::error("Error: {}", description);
}

void PrintVersionInfo() {
    const auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const auto glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    spdlog::info("GL Vendor: {}", vendor);
    spdlog::info("GL Renderer: {}", renderer);
    spdlog::info("GL Version: {}", version);
    spdlog::info("GLSL Version: {}", glslVersion);

    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
    spdlog::info("Available extensions:");
    for (int i = 0; i < nExtensions; i++) {
        const auto extStr = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        spdlog::info("{}", extStr);
    }

    int maxTessGenLevel = 0;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel);
    spdlog::info("Max teselation level: {}", maxTessGenLevel);

    int maxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
    spdlog::info("Max patch vertices: {}", maxPatchVertices);

    int maxShaderStorageBlockSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxShaderStorageBlockSize);
	spdlog::info("Max Shader Storage Block Size: {}", maxShaderStorageBlockSize);
	// int maxEvalOrder = 0;
    //glGetIntegerv(GL_MAX_EVAL_ORDER, &maxEvalOrder);
    //spdlog::info("Max eval order: {}", maxEvalOrder);

}
