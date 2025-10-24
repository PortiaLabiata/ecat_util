#pragma once
#include <vector>
#include <string>
#include <soem/soem.h>
#include "panel.hpp"

class PanelAdapters : public Panel {
public:
	PANEL_CONSTRUCTOR(PanelAdapters);

	const char *get_selected_iface() const {
		return adapters.size() > 0 ? adapters[selected_idx].c_str() : "";
	}

private:
	size_t selected_idx = 0;
	std::vector<std::string>adapters;

	void render_this() override {
		if (ImGui::BeginCombo("Available adapters", get_selected_iface())) {
			for (size_t i = 0; i < adapters.size(); i++) {
				const bool selected = (selected_idx == i);
				if (ImGui::Selectable(adapters[i].c_str(), selected)) {
					selected_idx = i;
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Scan")) {
			scan_adapters();
		}
		ImGui::SameLine();
		if (ImGui::Button("Select") && adapters.size() > 0) {
			deactivate();
		}
	}

	void scan_adapters() {
		ec_adaptert *adapter = nullptr;
		ec_adaptert *head = nullptr;
		head = adapter = ec_find_adapters();

		for (size_t i = 0; adapter != nullptr; i++) {
			if (i >= adapters.size()) {
				adapters.push_back(std::string(adapter->name));
			} else {
				adapters[i] = std::string(adapter->name);
			}
			adapter = adapter->next;
		}
		ec_free_adapters(head);
	}
};
