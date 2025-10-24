#pragma once
#include <ecat/core.hpp>
#include "panel.hpp"

class PanelSlaves : public Panel {
public:
	PANEL_CONSTRUCTOR(PanelSlaves);
	~PanelSlaves() override {}
	void attach_ecat(ECATMaster *_master) { master = _master; }
private:
	ECATMaster *master = nullptr;

	void render_this() override {
		if (!master) return;
		ImGui::BeginTable("Detected slaves", 3, ImGuiTableFlags_Borders);

		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Address");
		ImGui::TableSetupColumn("DType");
		ImGui::TableHeadersRow();

		for (auto slave : master->get_slaves()) {
			auto info = slave.get_slaveinfo();
			ImGui::TableNextColumn(); ImGui::Text("%s", info.name);
			ImGui::TableNextColumn(); ImGui::Text("%d", info.address);
			ImGui::TableNextColumn(); ImGui::Text("%d", info.device_type);
		}
		ImGui::EndTable();
	}
};
