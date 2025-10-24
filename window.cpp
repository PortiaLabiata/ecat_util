#include <iostream>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "window.hpp"

Window::Window(Vector2i dims, const char *title) {
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
	for (auto panel : panels) {
		panel->render();
	}
}

void Window::add_panel(Panel *panel) {
	panels.push_back(panel);
}
