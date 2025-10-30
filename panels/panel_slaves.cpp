#include <soem/soem.h>
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
		{"FMMU0 func.", EC::fmmu_to_str(info.FMMU0func)},
		{"FMMU1 func.", EC::fmmu_to_str(info.FMMU1func)},
		{"FMMU2 func.", EC::fmmu_to_str(info.FMMU2func)},
		{"FMMU3 func.", EC::fmmu_to_str(info.FMMU3func)},
	};
}

void EEPInfoView::render() {
	ImGui::BeginTable("EEPROM info", 2);
	for (auto entry : make_entries(obj.lock()->get_slaveinfo())) {
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
	if (obj.lock() == nullptr) goto end;

	for (const auto& slave : obj.lock()->get_slaves()) {
		auto info = slave->get_slaveinfo();
		TEXT_COLUMN("%s", info.name);
		TEXT_COLUMN("%d", info.configadr);
		TEXT_COLUMN("%d", info.eep_man);
		ImGui::TableNextColumn(); 
		if (ImGui::Button("Select")) {
			slave->update_state();
			update_views(slave);
		}
	}
end:
	ImGui::EndTable();
	for (const auto& view : attached_views) {
		view->render();
		ImGui::Separator();
	}
}

void PanelSlaves::render_this() {
	basic_master.render();
	table_master.render();
}

ec_state SlaveStateView::render_controls() {
	ec_state state = EC_STATE_NONE;
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Request state");
	if (ImGui::Button("Init", BUTTON_SIZE)) state = EC_STATE_INIT;
	ImGui::SameLine();
	if (ImGui::Button("Boot", BUTTON_SIZE)) state = EC_STATE_BOOT;
	if (ImGui::Button("PreOp", BUTTON_SIZE)) state = EC_STATE_PRE_OP;
	ImGui::SameLine();
	if (ImGui::Button("SafeOp", BUTTON_SIZE)) state = EC_STATE_SAFE_OP;
	if (ImGui::Button("Op", BUTTON_SIZE)) state = EC_STATE_OPERATIONAL;
	ImGui::EndGroup();
	return state;
}

void SlaveStateView::render_status() {
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Current state");
	ImGui::BeginTable("Current state", 2);
	ImGui::TableNextColumn(); ImGui::TextUnformatted("Link status");
	ImGui::TableNextColumn(); ImGui::TextUnformatted(obj.lock()->link_up() ? "UP" : "DOWN");
	ImGui::TableNextColumn(); ImGui::TextUnformatted("AL status");
	ImGui::TableNextColumn(); ImGui::TextUnformatted(ec_ALstatuscode2string(obj.lock()->get_slaveinfo().ALstatuscode));
	ImGui::TableNextColumn(); ImGui::TextUnformatted("ECAT state");
	ImGui::TableNextColumn(); ImGui::TextUnformatted(EC::ec_state_to_str(obj.lock()->get_state()));
	ImGui::TableNextColumn(); if (ImGui::Button("CoE")) asm("nop"); // Open CoE panel
	ImGui::EndTable();
	ImGui::EndGroup();
}

void SlaveStateView::render() {
	auto state = render_controls();
	ImGui::SameLine();
	render_status();

	if (state != EC_STATE_NONE && state != EC_STATE_ACK) {
		obj.lock()->set_state(state, 0);
	}
}

void BasicMasterView::render() {
	auto master_ptr = obj.lock();
	int total_current = 0;
	for (const auto& slave : master_ptr->get_slaves()) {
		total_current += slave->get_slaveinfo().Ebuscurrent;
	}

	ImGui::TextUnformatted("Link stats");
	ImGui::BeginTable("Link stats", 2);
	ImGui::TableNextColumn(); ImGui::TextUnformatted("#slaves");
	ImGui::TableNextColumn(); ImGui::Text("%ld", master_ptr->get_slaves().size());
	ImGui::TableNextColumn(); ImGui::TextUnformatted("Expected WKC");
	ImGui::TableNextColumn(); ImGui::Text("%d", master_ptr->get_expected_wc());
	ImGui::TableNextColumn(); ImGui::TextUnformatted("EBUS current, mA");
	ImGui::TableNextColumn(); ImGui::Text("%d", total_current);
	ImGui::EndTable();
}

void render_plot() {
	
}
