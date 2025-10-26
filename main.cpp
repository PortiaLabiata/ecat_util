#include "window.hpp"
#include "ecat/core.hpp"
#include <memory>
#include "panels/panels.hpp"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	ECATMaster *master = nullptr;
	PanelSlaves *slaves = nullptr;
	PanelConsole *console = nullptr;

	auto window = std::make_unique<Window>(ImVec2{600, 600}, "EtherCAT Utility");
	auto welcome = new WelcomePanel(ImVec2{600, 600});
	window->set_welcome_panel(welcome);

	while (!window->should_exit()) {
		window->loop();
		if (welcome && welcome->is_done()) {
			window->set_state(Window::State::main);
			console = new PanelConsole(ImVec2{600, 600});

			auto adapter_name = welcome->get_selected_iface();
			master = new ECATMaster(adapter_name);
			slaves = new PanelSlaves(ImVec2{600, 600}, master);

			window->add_panel(console);
			window->add_panel(slaves);

			delete welcome;
			welcome = nullptr;
		}
	}

	Panel::delete_all();
	delete master;
	return 0;
}
