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

	template <typename T> 
	void add_panel(std::unique_ptr<T>& panel) {
		panels.push_back(std::move(panel));		
	}
	void set_welcome_panel(std::unique_ptr<WelcomePanel>& panel) { welcome_screen = std::move(panel); }

	void set_state(State _state) { state = _state; }
	State get_state() { return state; }

private:
	State state;

	void render_panels();
	void create_panels();
	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	GLFWwindow *window;

	std::unique_ptr<WelcomePanel> welcome_screen;
	std::vector<std::unique_ptr<Panel>> panels;

	ECATMasterPtr master;
	PanelConsolePtr console;
	PanelSlavesPtr slaves;
};
