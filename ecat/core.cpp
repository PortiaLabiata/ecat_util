#include <feedback/logger.hpp>
#include "core.hpp"
#include "ecat/util.hpp"

ECATMaster::ECATMaster(const char *iface_name) {
	if (ecx_init(&ctx, iface_name) <= 0) {
		Log::errstream() << "Couldn't init iface, execute as root";
	}
	scan_bus();
}

ECATMaster::~ECATMaster() {
	ecx_close(&ctx);
}

void ECATMaster::scan_bus() {
	if (ecx_config_init(&ctx) <= 0) {
		Log::errstream() << "Couldn't config any slaves";
	}
	group = &ctx.grouplist[0];
	auto bsize = ecx_config_map_group(&ctx, reinterpret_cast<void*>(iomap), 0);
	if (bsize > static_cast<int>(IOMAP_SIZE)) {
		Log::errstream() << "IOMap buffer overflown by " << bsize - IOMAP_SIZE << " bytes";
	}
	ecx_configdc(&ctx);
	expected_wkc = (group->outputsWKC * 2) + group->inputsWKC;
	Log::logstream() << "Expected WKC: " << expected_wkc;

	//slaves.clear();
	for (int i = 0; i < ctx.slavecount; i++) {
		slaves.emplace_back(std::make_shared<ECATSlave>(&ctx, i));		
		Log::logstream() << "Detected slave with configaddr=" << ctx.slavelist[i].configadr;
	}
}

ECATSlave::ECATSlave(ecx_context *_ctx, size_t _index) : index(_index) {
	ctx = _ctx;	
}

void ECATSlave::set_state(ec_state state, unsigned int timeout_us) {
	if (!fsm_trans_valid(state)) return;
	ctx->slavelist[index].state = state;
	ecx_writestate(ctx, index);
	ecx_statecheck(ctx, index, state, timeout_us);
}

bool ECATSlave::fsm_trans_valid(ec_state state) {
	bool status = false;
	switch (get_state()) {
		case EC_STATE_INIT:
			status = (state == EC_STATE_PRE_OP || state == EC_STATE_BOOT);
			break;
		case EC_STATE_PRE_OP:
			status = (state == EC_STATE_INIT || state == EC_STATE_SAFE_OP);
			break;
		case EC_STATE_SAFE_OP:
			status = (state == EC_STATE_OPERATIONAL || state == EC_STATE_PRE_OP);
			break;
		case EC_STATE_OPERATIONAL:
			status = (state == EC_STATE_SAFE_OP || state == EC_STATE_PRE_OP \
							|| state == EC_STATE_INIT);
			break;
		case EC_STATE_NONE:
		case EC_STATE_ACK:
		default:
			break;
	}
	if (status) {
		Log::logstream() << "Slave " << ctx->slavelist[index].configadr << " in state " << EC::ec_state_to_str(state);
	} else {
		Log::wrnstream() << "Attempt to perform illegal transition for slave " << ctx->slavelist[index].configadr;
	}
	return status;
}

// Inefficient if there are many slaves, but will do for now
// for the sake of clean architecture.
ec_state ECATSlave::update_state() {
	if (ecx_readstate(ctx) <= 0) {
		throw BusReadError(ecx_elist2string(ctx));
	}
	return static_cast<ec_state>(ctx->slavelist[index].state);
}

// Returns last RECORDED slave state
ec_state ECATSlave::get_state() {
	return static_cast<ec_state>(ctx->slavelist[index].state);
}
