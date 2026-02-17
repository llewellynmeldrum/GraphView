#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdlib>
#include <print>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
	#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "log.hpp"

using namespace std;

namespace IG = ImGui;
// OPENGL3
struct SharedContext {
	float mainScale;
	GLFWwindow *viewport = nullptr;
	const char *glslVersion = nullptr;
	ImVec4 bgColor;
};

constexpr bool ENABLE_VSYNC = true;

// GLFW3
struct GLFWHandler {
	SharedContext &shared;

	const char *getGLSLVersion();
	static void error_callback(int error, const char* description);
	GLFWHandler(auto& _shared): shared(_shared) {}


	void handleInputs();
	void render();
	void init() {
		glfwSetErrorCallback(error_callback);
		if (!glfwInit()) {
			LOG::err("Failed to initialize glfw.");
			std::exit(EXIT_FAILURE);
		}
		auto primaryMonitor = glfwGetPrimaryMonitor();

		shared.glslVersion = getGLSLVersion();
		shared.mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(primaryMonitor);
		int screenWidth = glfwGetVideoMode(primaryMonitor)->width;
		int screenHeight = glfwGetVideoMode(primaryMonitor)->height;

		shared.viewport = glfwCreateWindow(
		                      (screenWidth / 2.0) * shared.mainScale,
		                      screenHeight * shared.mainScale,
		                      "Viewport title", nullptr, nullptr);
		glfwSetWindowPos(shared.viewport, 0, 0);

		if (!shared.viewport) {
			LOG::err("Failed to initialize glfw - viewport (window) ptr is null.");
			std::exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(shared.viewport);
		glfwSwapInterval(ENABLE_VSYNC); // enables vsync
	}
};


struct ImGuiHandler {
	SharedContext &shared;
	ImGuiContext *context = nullptr;
	ImVec4 bgColor;
	ImGuiHandler(SharedContext& _shared): shared(_shared) { }
	inline void init() {
		IMGUI_CHECKVERSION();
		context = IG::CreateContext();
		auto& io = IG::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		IG::StyleColorsDark();
		ImGuiStyle& style = IG::GetStyle();
		style.ScaleAllSizes(shared.mainScale);
		style.FontScaleDpi = shared.mainScale;

		ImGui_ImplGlfw_InitForOpenGL(shared.viewport, true);
		ImGui_ImplOpenGL3_Init(shared.glslVersion);
		bgColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	}
	void composeUI();
};

void GLFWHandler::handleInputs() {
	glfwPollEvents();
}

int msDelay = 10.0;

void ImGuiHandler::composeUI() {
	auto& io = IG::GetIO();
	if (glfwGetWindowAttrib(shared.viewport, GLFW_ICONIFIED) != 0) {
		ImGui_ImplGlfw_Sleep(10);
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	IG::NewFrame();


	{
		static float f = 0.0f;
		static int counter = 0;

		IG::Begin("Configuration");                          // Create a window called "Hello, world!" and append into it.

		IG::Text("Text.");               // Display some text (you can use a format strings too)
		IG::SliderInt("msDelay", &msDelay, 0, 50);

		if (IG::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		IG::SameLine();
		IG::Text("counter = %d", counter);

		IG::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		IG::End();
	}
	IG::Render();
}
void GLFWHandler::render() {
	int display_w, display_h;
	glfwGetFramebufferSize(shared.viewport, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(IG::GetDrawData());
	glfwSwapBuffers(shared.viewport);
}

// certain things need to interop
bool shouldClose(auto* window) {
	return glfwWindowShouldClose(window);
}




void cleanupScene(SharedContext& shared);

int main() {
	SharedContext 	shared;
	GLFWHandler 	platform(shared);
	ImGuiHandler 		ui(shared);

	platform.init();
	ui.init();
	while (!shouldClose(shared.viewport)) {
		platform.handleInputs();
		ui.composeUI();
		platform.draw();
	}
	cleanupScene(shared);
}

void cleanupScene(SharedContext& shared) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	IG::DestroyContext();
	glfwDestroyWindow(shared.viewport);
	glfwTerminate();
}

inline void GLFWHandler::error_callback(int error, const char* description) {
	LOG::err("GLFW Error {}: {}\n", error, description);
}
const char *GLFWHandler::getGLSLVersion() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
	const char *glslVersion = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
	const char *glslVersion = "#version 300 es";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char *glslVersion = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char *glslVersion = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	return glslVersion;
}
