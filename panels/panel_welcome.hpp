#pragma once
#include <vector>
#include <string>
#include <soem/soem.h>
#include "panel.hpp"

class WelcomePanel : public Panel {
public:
	PANEL_CONSTRUCTOR(WelcomePanel);
	WelcomePanel(ImVec2 size) : Panel(size, "Welcome") { scan_adapters(); }
	~WelcomePanel() override {};

	const char *get_selected_iface() const {
		return adapters.size() > 0 ? adapters[selected_idx].c_str() : "";
	}
	
	bool is_done() const { return done; }

private:
	size_t selected_idx = 0;
	std::vector<std::string>adapters;
	bool done = false;

	void render_this() override; 

	void scan_adapters(); 
};
