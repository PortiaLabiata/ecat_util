#pragma once
#include <vector>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include "panels/panels.hpp"

class Window {
public:
	Window(ImVec2 dims, const char *title);
	~Window();

	enum class State {
		welcome = 0,
		main
	};

	void loop();
	bool should_exit() { return glfwWindowShouldClose(window); }

	void add_panel(Panel *panel);
	void set_welcome_panel(WelcomePanel *panel) { welcome_screen = panel; }

	void set_state(State _state) { state = _state; }
	State get_state() { return state; }

private:
	State state;

	void render_panels();
	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	GLFWwindow *window;
	Panel *welcome_screen;
	std::vector<Panel*> panels;
};
