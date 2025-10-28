#include <memory>
#include <feedback/logger.hpp>
#include "window.hpp"
#include "ecat/core.hpp"
#include "panels/panels.hpp"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	auto window = std::make_unique<Window>(ImVec2{600, 600}, "EtherCAT Utility");
	auto welcome = std::make_unique<WelcomePanel>(ImVec2{600, 600});
	window->set_welcome_panel(welcome);
	Log::logstream() << "Starting application";

	while (!window->should_exit()) {
		window->loop();
	}
	return 0;
}
