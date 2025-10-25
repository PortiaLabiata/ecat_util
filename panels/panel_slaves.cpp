#include "panel_slaves.hpp"

// This is horrifying, but is the best I could come up with so far
std::vector<Entry> EEPInfoView::make_entries(const ec_slavet &info) {
	return {
		{"Name", info.name},
		{"Manufacturer", info.eep_man},
		{"Rev .ID", info.eep_rev},
		{"ID", info.eep_id},
		{"SN", info.eep_ser},
		{"Itype", info.Itype},
		{"Obits", info.Obits},
		{"Ibits", info.Ibits},
		{"Act. ports", info.activeports},
		{"FMMU0 func.", fmmu_to_str(info.FMMU0func)},
		{"FMMU1 func.", fmmu_to_str(info.FMMU1func)},
		{"FMMU2 func.", fmmu_to_str(info.FMMU2func)},
		{"FMMU3 func.", fmmu_to_str(info.FMMU3func)},
	};
}

void EEPInfoView::render() {
	ImGui::BeginTable("EEPROM info", 2);
	for (auto entry : make_entries(obj->get_slaveinfo())) {
		std::ostringstream s;
		entry.print_to(s);

		ImGui::TableNextColumn(); ImGui::TextUnformatted(entry.label.c_str());
		ImGui::TableNextColumn(); ImGui::TextUnformatted(s.str().c_str());
	}
	ImGui::EndTable();
}

void TableMasterView::render() {
	ImGui::BeginTable("Detected slaves", 4, ImGuiTableFlags_Borders);

	ImGui::TableSetupColumn("Name");
	ImGui::TableSetupColumn("Address");
	ImGui::TableSetupColumn("DType");
	ImGui::TableSetupColumn("Select");
	ImGui::TableHeadersRow();
	if (!obj) goto end;

	for (auto slave : obj->get_slaves()) {
		auto info = slave.get_slaveinfo();
		TEXT_COLUMN("%s", info.name);
		TEXT_COLUMN("%d", info.configadr);
		TEXT_COLUMN("%d", info.eep_man);
		ImGui::TableNextColumn(); 
		if (ImGui::Button("Select")) {
			update_views(&slave);
		}
	}
end:
	ImGui::EndTable();
}

void PanelSlaves::render_this() {
	table_master.render();
	eep_info.render();
}

void PanelSlaves::render_controls() {

}
