#pragma once
#include <inttypes.h>
#include <vector>
#include <soem/soem.h>

class ECATSlave {
	friend class ECATMaster;
public:
	ECATSlave(ecx_context *ctx, size_t index);
	~ECATSlave() = default;

	struct basicinfo_t {
		const char *name;
		uint32_t vendor;
		uint16_t device_type;
		uint16_t address;
		ec_state state;
	};

	const basicinfo_t &get_slaveinfo() const {
		return basicinfo;
	}

	void set_state(ec_state state, unsigned int timeout_ms);
	ec_state get_state();

private:
	void fill_from_ecx(const ec_slavet &slave);
	basicinfo_t basicinfo;

	// I hate it, but should work, since ctx is not supposed to be null
	// until the execution stops
	ecx_context *ctx;
	const size_t index;
};

class ECATMaster {
public:
	ECATMaster(const char *iface_name);
	~ECATMaster();

	void scan_bus();
private:
	static const size_t IOMAP_SIZE = 4096;
	
	// Will do for now, if there are performance issues, move to smart pointers
	std::vector<ECATSlave> slaves;

	ecx_context ctx;
	ec_groupt *group;
	uint8 iomap[IOMAP_SIZE];
	int expected_wkc;
};

/* Exceptions */
struct IfaceError {
	const char *iface;
};

struct SlaveConfigError {
	const char *circumstances;
};

struct BusReadError {
	const char *circumstances;
};
