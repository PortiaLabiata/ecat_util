#include <cstdarg>
#include "panel_console.hpp"

void PanelConsole::log(PanelConsole::LogLevel level, const char *fmt, ...) {
	std::va_list args;
	va_start(args, fmt);
	char text[BUFFER_SIZE];
	std::snprintf(text, BUFFER_SIZE, fmt, args);
	contents.emplace_back(ConsoleEntry(level, text));
	va_end(args);
}

void PanelConsole::render_this() {
	if (ImGui::Button("Clear")) {
		contents.clear();
	}
	for (auto entry : contents) {
		ImGui::TextColored(entry.get_color(), "%s", entry.text.c_str());
	}
}

