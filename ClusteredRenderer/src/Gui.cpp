#include "Gui.hpp"

static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
static float baseFontSize = 26.0f;						 // 13.0f is the size of the default font. Change to the font size you use.
static float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

void InitStyle(float scale) {
	static float oldScale = 1.0f;
	if (oldScale == scale)
		return;

	ImGuiStyle& style = ImGui::GetStyle();
	style = ImGuiStyle{};

	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	style.WindowMenuButtonPosition = ImGuiDir_None;

	style.ScaleAllSizes(scale);
}

ImFontAtlas* CreateFontAtlas(float scale) {
	ImFontAtlas* fontAtlas = new ImFontAtlas{};

	auto fontCfg = ImFontConfig();
	fontCfg.SizePixels = std::roundf(baseFontSize);

	fontAtlas->AddFontDefault(&fontCfg);
	// merge in icons from Font Awesome
	static ImFontConfig icons_config{};
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = scale * iconFontSize;
	fontAtlas->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, std::roundf( scale * iconFontSize ), &icons_config, icons_ranges);
	// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid
	fontAtlas->Build();

	return fontAtlas;
}

Gui::Gui(const Window& window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	InitStyle(1.0f);

	auto* fontAtlas = CreateFontAtlas(1.0f);
	io.Fonts = fontAtlas;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
	ImGui_ImplOpenGL3_Init(config::glslVersion);
}

Gui::~Gui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::NewFrame() const {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Gui::Render(const Window& window) const {
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window.glfwWindow(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}