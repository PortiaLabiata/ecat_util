#pragma once
#include <string>
#include <sstream>
#include <variant>
#include <ecat/util.hpp>
#include <ecat/core.hpp>
#include "panel.hpp"

#define TEXT_COLUMN(__FMT__, __ARGS__) ImGui::TableNextColumn(); ImGui::Text(__FMT__, __ARGS__)

static ECATSlaveDummy dummy;

struct Entry {
	std::string label;
	std::variant<const char*, uint8_t, uint16_t, uint32_t> value;

	void print_to(std::ostringstream &s) {
		std::visit([&s](const auto& v){ s << v; }, value);
	}
};

class EEPInfoView : public SlaveView {
public:
	void render() override;
private:
	std::vector<Entry> make_entries(const ec_slavet &info);
};

class TableMasterView : public MasterView {
public:
	void render() override;
};

class PanelSlaves : public Panel {
public:
	PanelSlaves(ImVec2 size, ECATMaster *master) : Panel(size, "Slaves") {
		table_master.set_object(master);
		eep_info = table_master.construct_slave_view<EEPInfoView>();
		table_master.update_views(&dummy);
	}

	~PanelSlaves() override {}; 
private:
	TableMasterView table_master;
	EEPInfoView *eep_info;

	ECATSlave *selected_slave = static_cast<ECATSlave*>(&dummy);
	static const size_t slave_info_size = 15;

	void render_this() override; 
	void render_controls(); 
};
