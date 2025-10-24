#pragma once
#include <format>
#include <ecat/util.hpp>
#include <utility>
#include <ecat/core.hpp>
#include "panel.hpp"

#define TEXT_COLUMN(__FMT__, __ARGS__) ImGui::TableNextColumn(); ImGui::Text(__FMT__, __ARGS__)
#define BEGIN_ENTRIES auto entry = output
#define ENTRY(__LABEL__, __VALUE__) *(entry++) = std::make_pair(__LABEL__, std::format("{}", __VALUE__))

static ECATSlaveDummy dummy;

// This is horrifying, but I couldn't think of anything else yet. If you figure it out - 
// feel free to submit a pull request.
using Entry = std::pair<std::string, std::string>;
static void print_slaveinfo(Entry output[], const ec_slavet &slave) {
	BEGIN_ENTRIES;
	ENTRY("Name", slave.name);
	ENTRY("Manufacturer", slave.eep_man);
	ENTRY("Rev. ID", slave.eep_rev);
	ENTRY("ID", slave.eep_id);
	ENTRY("SN", slave.eep_ser);
	ENTRY("Itype", slave.Itype);
	ENTRY("Dtype", slave.Dtype);
	ENTRY("Obits", slave.Obits);
	ENTRY("Ibits", slave.Ibits);
	ENTRY("Act. ports", slave.activeports & 0x01);
	ENTRY("FMMU0 func.", fmmu_to_str(slave.FMMU0func & 0x01).c_str());
	ENTRY("FMMU1 func.", fmmu_to_str(slave.FMMU1func & 0x01).c_str());
	ENTRY("FMMU2 func.", fmmu_to_str(slave.FMMU2func & 0x01).c_str());
	ENTRY("FMMU3 func.", fmmu_to_str(slave.FMMU3func & 0x01).c_str());
	ENTRY("Mbx protocols: ", mb_to_str(slave.mbx_proto).c_str());
}

class PanelSlaves : public Panel {
public:
	PanelSlaves(ImVec2 size, const char *title) : Panel(size, title) {
		selected_slave = static_cast<ECATSlave*>(&dummy);	
		print_slaveinfo(slave_info, dummy.get_slaveinfo());
	}

	~PanelSlaves() override {}
	void attach_ecat(ECATMaster *_master) { master = _master; }
private:
	ECATMaster *master = nullptr;
	ECATSlave *selected_slave = nullptr;
	static const size_t slave_info_size = 15;
	Entry slave_info[slave_info_size];

	void render_this() override {
		ImGui::BeginTable("Detected slaves", 4, ImGuiTableFlags_Borders);

		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Address");
		ImGui::TableSetupColumn("DType");
		ImGui::TableSetupColumn("Select");
		ImGui::TableHeadersRow();
		if (!master) goto print_info;

		for (auto slave : master->get_slaves()) {
			auto info = slave.get_slaveinfo();
			TEXT_COLUMN("%s", info.name);
			TEXT_COLUMN("%d", info.configadr);
			TEXT_COLUMN("%d", info.eep_man);
			ImGui::TableNextColumn(); 
			if (ImGui::Button("Select")) {
				print_slaveinfo(slave_info, info);
			}
		}
print_info:
		ImGui::EndTable();
		render_slaveinfo();
		render_controls();
	}

	void render_slaveinfo() {
		ImGui::BeginTable("Slave info 1", 2);
		for (const auto &entry : slave_info) {
			TEXT_COLUMN("%s", entry.first.c_str());
			TEXT_COLUMN("%s", entry.second.c_str());
		}
		ImGui::EndTable();
	}

	void render_controls() {

	}
};
