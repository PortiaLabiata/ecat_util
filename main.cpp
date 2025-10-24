#include "window.hpp"
#include "ecat/core.hpp"
#include <memory>
#include "panels/panels.hpp"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	ECATMaster *master = nullptr;

	auto window = std::make_unique<Window>(ImVec2{600, 600}, "EtherCAT Utility");
	auto adapters = std::make_unique<PanelAdapters>(ImVec2{200, 200}, "Network adapters");
	auto slaves = std::make_unique<PanelSlaves>(ImVec2{200, 200}, "Slaves");
	auto console = std::make_unique<PanelConsole>(ImVec2{200, 200});

	window->add_panel(adapters.get());
	window->add_panel(slaves.get());
	window->add_panel(console.get());

	while (!window->should_exit()) {
		window->loop();

		if (adapters->was_iface_changed()) {
			adapters->reset_flag();
			master = new ECATMaster(adapters->get_selected_iface());
			slaves->attach_ecat(master);
			master->scan_bus();
		}
	}

	delete master;
	return 0;
}
