#include "core.hpp"
#include "panels/panel_console.hpp"

ECATMaster::ECATMaster(const char *iface_name) {
	if (ecx_init(&ctx, iface_name) <= 0) {
		PanelConsole::get_singleton()->log(PanelConsole::LogLevel::error, "Couldn't init iface, run as root");
	}
}

ECATMaster::~ECATMaster() {
	ecx_close(&ctx);
}

void ECATMaster::scan_bus() {
	if (ecx_config_init(&ctx) <= 0) {
			PanelConsole::get_singleton()->log(PanelConsole::LogLevel::error, "Couldn't config any slaves");
	}
	group = &ctx.grouplist[0];
	if (ecx_config_map_group(&ctx, reinterpret_cast<void*>(iomap), 0) > static_cast<int>(IOMAP_SIZE)) {
			PanelConsole::get_singleton()->log(PanelConsole::LogLevel::error, "IOMap buffer overflow");
	}
	ecx_configdc(&ctx);
	expected_wkc = (group->outputsWKC * 2) + group->inputsWKC;

	slaves.clear();
	for (int i = 0; i < ctx.slavecount; i++) {
		slaves.emplace_back(ECATSlave(&ctx, i));		
	}
}

ECATSlave::ECATSlave(ecx_context *_ctx, size_t _index) : index(_index) {
	ctx = _ctx;	
}

void ECATSlave::set_state(ec_state state, unsigned int timeout_us) {
	ctx->slavelist[index].state = state;
	ecx_writestate(ctx, index);
	ecx_statecheck(ctx, index, state, timeout_us);
}

// Inefficient if there are many slaves, but will do for now
// for the sake of clean architecture.
ec_state ECATSlave::get_state() {
	if (ecx_readstate(ctx) <= 0) {
		throw BusReadError(ecx_elist2string(ctx));
	}
	return static_cast<ec_state>(ctx->slavelist[index].state);
}
