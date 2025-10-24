#include "window.hpp"
#include "vector.hpp"
#include "panels.hpp"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	Window *window = new Window(
		Vector2i{600, 600},
		"EtherCAT Utility"
	);

	Panel *adapters = new PanelAdapters(Vector2i{200, 200}, "Network adapters");
	Panel *slaves = new PanelSlaves(Vector2i{200, 200}, "Slaves");

	window->add_panel(adapters);
	window->add_panel(slaves);
	while (!window->should_exit()) {
		window->loop();
	}
	return 0;
}
