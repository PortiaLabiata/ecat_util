#pragma once
#include <vector>
#include <string>
#include <soem/soem.h>
#include "panel.hpp"

class WelcomePanel : public Panel {
public:
	PANEL_CONSTRUCTOR(WelcomePanel);
	~WelcomePanel() override {}

	const char *get_selected_iface() const {
		return adapters.size() > 0 ? adapters[selected_idx].c_str() : "";
	}
	
	bool is_done() const { return done; }

private:
	size_t selected_idx = 0;
	std::vector<std::string>adapters;
	bool done = false;

	void render_this() override {
		ImGui::SetWindowSize(ImVec2{334, 114});
		ImGui::Text("Welcome! Please select network adapter, \nthat you will use for EtherCAT communication");
		if (ImGui::BeginCombo("Ifaces", get_selected_iface())) {
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
			done = true;
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
