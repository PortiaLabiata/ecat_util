#include <feedback/logger.hpp>
#include <cstring>
#include "core.hpp"
#include "ecat/util.hpp"

ECATMaster::ECATMaster(const char *iface_name) {
	if (ecx_init(&ctx, iface_name) <= 0) {
		Log::errstream() << "Couldn't init iface, execute as root";
	}
	iothread = std::thread(&ECATMaster::iothread_func, this);
	queue.push(ECATMaster::Action::ConfigSlaves);
	queue.push(ECATMaster::Action::ConfigDC);
}

ECATMaster::~ECATMaster() {
	auto guard = ContextGuard(&master_context);
	master_context.stop = true;
	// FIXME: potential segfault
	ecx_close(&ctx);
}

void ECATMaster::scan_bus() {
	if (ecx_config_init(&ctx) <= 0) {
		Log::errstream() << "Couldn't config any slaves";
		auto error_str = ecx_elist2string(&ctx);
		Log::errstream() << "SOEM error: " << (std::strcmp(error_str, "") != 0 ? error_str : "None");
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
	auto err = ecx_readstate(ctx);
	if (err <= 0) {
		Log::errstream() << "Couldn't read state or state is invalid";
	}
	return static_cast<ec_state>(ctx->slavelist[index].state);
}

// Returns last RECORDED slave state
ec_state ECATSlave::get_state() {
	return static_cast<ec_state>(ctx->slavelist[index].state);
}

void ECATMaster::iothread_func() {
	while (1) {
		{ // ContextGuard
			auto guard = ContextGuard(&master_context);
			if (master_context.stop) {
				break;
			}
		} // ContextGuard

		while (queue.size() > 0) {
			auto action = queue.pop();
			execute_action(action);
		}

		for (const auto& slave : slaves) {
			slave->exhaust_queue();	
		}
	}
}

void ECATMaster::execute_action(ECATMaster::Action action) {
	auto guard = ContextGuard(&master_context);
	switch (action) {
		case ECATMaster::Action::ConfigSlaves: {
			Log::logstream() << "Scanning bus...";
			if (ecx_config_init(&ctx) <= 0) {
				Log::errstream() << "Could not config any slaves";
			}
			group = &ctx.grouplist[0];
			auto bsize = ecx_config_map_group(&ctx, static_cast<void*>(iomap), 0);
			if (bsize > static_cast<int>(IOMAP_SIZE)) {
				Log::errstream() << "IOMap buffer overflown by " << bsize - IOMAP_SIZE << " bytes";
			}

			slaves.clear();
			for (int i = 0; i < ctx.slavecount; i++) {
				slaves.emplace_back(std::make_shared<ECATSlave>(&ctx, i));		
				Log::logstream() << "Detected slave with configaddr=" << ctx.slavelist[i].configadr;
			}
			break;
		} // Action::ConfigSlaves
		case ECATMaster::Action::ConfigDC: {
			if (ecx_configdc(&ctx) <= 0) {
				Log::errstream() << "Could not config DC";
			}

			expected_wkc = (group->outputsWKC * 2) + group->inputsWKC;
			Log::logstream() << "Expected WKC: " << expected_wkc;
			break;
		} // Action::ConfigDC
		case ECATMaster::Action::UpdateState: {
			if (ecx_readstate(&ctx) <= 0) {
				Log::wrnstream() << "Some slaves are in invalid state";
				break;
			}
			break;
		} // Action::UpdateState
		default:
			Log::errstream() << "Invalid action code " << static_cast<int>(action);
	}
}

void ECATSlave::exhaust_queue() {
	while (queue.size() > 0) {
		execute_action(queue.pop());	
	}
}

void ECATSlave::execute_action(ECATSlave::Action action) {
	auto guard = ContextGuard(&slave_context);
	switch (action) {
		case ECATSlave::Action::SetState:
			set_state(slave_context.state, 20000);
			break;
		default:
			break;
	}
}
