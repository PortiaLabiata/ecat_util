#include <iostream>
#include "panel_console.hpp"

void ConsoleLogView::render() {
	for (auto entry : obj->get_messages()) {
		ImGui::TextColored(get_color(entry.severity), "%s", entry.message.c_str());
	}
}
