#include "core.hpp"

ECATMaster::ECATMaster(const char *iface_name) {
	if (ecx_init(&ctx, iface_name) <= 0) {
		throw IfaceError(iface_name);
	}
}

ECATMaster::~ECATMaster() {
	ecx_close(&ctx);
}

void ECATMaster::scan_bus() {
	if (ecx_config_init(&ctx) <= 0) {
		throw SlaveConfigError("Couldn't config slaves");
	}
	group = &ctx.grouplist[0];
	if (ecx_config_map_group(&ctx, reinterpret_cast<void*>(iomap), 0) > static_cast<int>(IOMAP_SIZE)) {
		throw SlaveConfigError("IOMap overflow");
	}
	ecx_configdc(&ctx);
	expected_wkc = (group->outputsWKC * 2) + group->inputsWKC;

	for (int i = 0; i < ctx.slavecount; i++) {
		slaves.emplace_back(ECATSlave(&ctx, i));		
	}
}

ECATSlave::ECATSlave(ecx_context *ctx, size_t _index) : index(_index) {
	ec_slavet *slave = &ctx->slavelist[index];
	basicinfo.name = slave->name;
	basicinfo.vendor = slave->eep_man;
	basicinfo.device_type = slave->Dtype;
	basicinfo.address = slave->configadr;
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
