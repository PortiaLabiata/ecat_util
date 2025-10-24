#pragma once
#include <cstdio>
#include "slave/core.hpp"
#include "panel.hpp"

class PanelSlaves : public Panel {
public:
	PANEL_CONSTRUCTOR(PanelSlaves);
private:
	void render_this() override {
		ImGui::BeginTable("Detected slaves", 3, ImGuiTableFlags_Borders);
		static bool selected[10];
		for (int i = 0; i < 10; i++) {
			char label[32];
			std::sprintf(label, "Item %d", i);
			ImGui::TableNextColumn();
			ImGui::Selectable(label, &selected[i]); // FIXME-TABLE: Selection overlap
        }
		ImGui::EndTable();
	}
};
