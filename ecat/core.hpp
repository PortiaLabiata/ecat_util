#pragma once
#include <inttypes.h>
#include <vector>
#include <cstdio>
#include <soem/soem.h>

class ECATSlave {
	friend class ECATMaster;
public:
	ECATSlave(ecx_context *ctx, size_t index);
	~ECATSlave() = default;

	const ec_slavet &get_slaveinfo() const {
		return ctx->slavelist[index];
	}

	void set_state(ec_state state, unsigned int timeout_ms);
	ec_state get_state();

private:
	void fill_from_ecx(const ec_slavet &slave);

	// I hate it, but should work, since ctx is not supposed to be null
	// until the execution stops
	ecx_context *ctx;
	const size_t index;
};

class ECATSlaveDummy : public ECATSlave {
public:
	ECATSlaveDummy() : ECATSlave(nullptr, 0) {
		std::sprintf(mock_ctx.slavelist[0].name, "-");
	}

	~ECATSlaveDummy() = default;

	const ec_slavet &get_slaveinfo() const {
		return mock_ctx.slavelist[0];	
	}
private:
	ecx_context mock_ctx;
};

class ECATMaster {
public:
	ECATMaster(const char *iface_name);
	~ECATMaster();

	void scan_bus();
	const std::vector<ECATSlave> &get_slaves() const {
		return slaves;
	}

	int get_expected_wc() const { return expected_wkc; }

private:
	static const size_t IOMAP_SIZE = 4096;
	
	// Will do for now, if there are performance issues, move to smart pointers
	std::vector<ECATSlave> slaves;

	ecx_context ctx;
	ec_groupt *group;
	uint8 iomap[IOMAP_SIZE];
	int expected_wkc;
};

struct BusReadError {
	const char *circumstances;
};
