#pragma once
#include <vector>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include "panels/panel.hpp"

class Window {
public:
	Window(ImVec2 dims, const char *title);
	~Window();

	void loop();
	bool should_exit() { return glfwWindowShouldClose(window); }
	void add_panel(Panel *panel);
private:
	void render_panels();
	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	GLFWwindow *window;
	std::vector<Panel*> panels;
};
