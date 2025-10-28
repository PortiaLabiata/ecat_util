#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <feedback/logger.hpp>
#include "window.hpp"

Window::Window(ImVec2 dims, const char *title) {
	if (!glfwInit()) {
		return;
	}
	window = glfwCreateWindow(dims.x, dims.y, title, nullptr, nullptr);
	if (!window) {
		return;
	}
	glfwMakeContextCurrent(window);
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	state = State::welcome;
}

Window::~Window() {
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::loop() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	glfwPollEvents();

	render_panels();	

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    	
	glfwSwapBuffers(window);
}

void Window::render_panels() {
	switch (state) {
		case State::welcome:
			welcome_screen->render();
			if (welcome_screen->is_done()) {
				create_panels();
				state = State::main;
				welcome_screen.reset();
			}
			break;
		case State::main:
		default:
			for (const auto &panel : panels) {
				panel->render();
			}
			break;
	}
}

void Window::create_panels() {
	const char *adapter_name = welcome_screen->get_selected_iface();
	Log::logstream() << "Selected iface " << adapter_name;
	master = std::make_shared<ECATMaster>(adapter_name);
	console = std::make_unique<PanelConsole>(ImVec2{600, 600});
	slaves = std::make_unique<PanelSlaves>(ImVec2{600, 600}, master);

	add_panel(console);
	add_panel(slaves);
}
