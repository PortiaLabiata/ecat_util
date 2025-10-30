#pragma once
#include <string>
#include <sstream>
#include <variant>
#include <ecat/util.hpp>
#include <ecat/core.hpp>
#include "panel.hpp"

#define TEXT_COLUMN(__FMT__, __ARGS__) ImGui::TableNextColumn(); ImGui::Text(__FMT__, __ARGS__)

static std::shared_ptr<ECATSlaveDummy> dummy = std::make_shared<ECATSlaveDummy>();

struct Entry {
	std::string label;
	std::variant<const char*, uint8_t, uint16_t, uint32_t> value;

	void print_to(std::ostringstream &s) {
		std::visit([&s](const auto& v){ s << v; }, value);
	}
};

class EEPInfoView : public SlaveView {
public:
	EEPInfoView() = default;
	void render() override;
private:
	std::vector<Entry> make_entries(const ec_slavet &info);
};

class SlaveStateView : public SlaveView {
public:
	void render() override;
private:
	ec_state render_controls();
	void render_status();
	std::vector<Entry> make_entries(const ec_slavet &info);

	static constexpr ImVec2 BUTTON_SIZE = ImVec2{50, 30}; 
};

class TableMasterView : public MasterView {
public:
	void render() override;
};

class BasicMasterView : public MasterView {
public:
	void render() override;
};

class PanelSlaves : public Panel {
public:
	PanelSlaves(ImVec2 size, std::shared_ptr<ECATMaster> master) : Panel(size, "Slaves") {
		table_master.set_object(master);
		basic_master.set_object(master);

		table_master.construct_slave_view<EEPInfoView>();
		table_master.construct_slave_view<SlaveStateView>();

		table_master.update_views(dummy);
	}

	~PanelSlaves() override {}; 
private:
	BasicMasterView basic_master;
	TableMasterView table_master;
	std::weak_ptr<ECATSlave> selected_slave = dummy;
	static const size_t slave_info_size = 15;

	void render_this() override; 
	void render_plot();
};
using PanelSlavesPtr = std::unique_ptr<PanelSlaves>;
