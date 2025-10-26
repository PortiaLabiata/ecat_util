#include <feedback/logger.hpp>
#include "core.hpp"

ECATMaster::ECATMaster(const char *iface_name) {
	if (ecx_init(&ctx, iface_name) <= 0) {
		EC::errstream() << "Couldn't init iface, execute as root";
	}
	scan_bus();
}

ECATMaster::~ECATMaster() {
	ecx_close(&ctx);
}

void ECATMaster::scan_bus() {
	if (ecx_config_init(&ctx) <= 0) {
		EC::errstream() << "Couldn't config any slaves";
	}
	group = &ctx.grouplist[0];
	auto bsize = ecx_config_map_group(&ctx, reinterpret_cast<void*>(iomap), 0);
	if (bsize > static_cast<int>(IOMAP_SIZE)) {
		EC::errstream() << "IOMap buffer overflown by " << bsize - IOMAP_SIZE << " bytes";
	}
	ecx_configdc(&ctx);
	expected_wkc = (group->outputsWKC * 2) + group->inputsWKC;
	EC::logstream() << "Expected WKC: " << expected_wkc;

	slaves.clear();
	for (int i = 0; i < ctx.slavecount; i++) {
		slaves.emplace_back(ECATSlave(&ctx, i));		
		EC::logstream() << "Detected slave with configaddr=" << ctx.slavelist[i].configadr;
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
