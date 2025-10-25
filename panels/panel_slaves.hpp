#pragma once
#include <string>
#include <sstream>
#include <variant>
#include <ecat/util.hpp>
#include <ecat/core.hpp>
#include "panel.hpp"

#define TEXT_COLUMN(__FMT__, __ARGS__) ImGui::TableNextColumn(); ImGui::Text(__FMT__, __ARGS__)
#define BEGIN_ENTRIES auto entry = output
#define ENTRY(__LABEL__, __VALUE__) *(entry++) = std::make_pair(__LABEL__, std::format("{}", __VALUE__))

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
	EEPInfoView() = default;
	~EEPInfoView() override {}

	void render() override;
private:
	std::vector<Entry> make_entries(const ec_slavet &info);
};

// This is horrifying, but I couldn't think of anything else yet. If you figure it out - 
// feel free to submit a pull request.

class PanelSlaves : public Panel {
public:
	PanelSlaves(ImVec2 size, const char *title) : Panel(size, title) {
		selected_slave = static_cast<ECATSlave*>(&dummy);	
		info_view.set_slave(selected_slave);
	}

	~PanelSlaves() override {}; 
	void attach_ecat(ECATMaster *_master) { master = _master; }
private:
	ECATMaster *master = nullptr;
	ECATSlave *selected_slave = nullptr;
	static const size_t slave_info_size = 15;
	EEPInfoView info_view;

	void render_this() override; 
	void render_controls(); 
};
