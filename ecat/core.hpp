#pragma once
#include <memory>
#include <queue>
#include <thread>
#include <inttypes.h>
#include <vector>
#include <cstdio>
#include <soem/soem.h>

template <typename T>
struct ActionQueue {
	std::mutex mtx;
	std::queue<T> queue;

	void push(T action) {
		mtx.lock();
		queue.push(action);
		mtx.unlock();
	}

	T pop() {
		mtx.lock();
		auto temp = queue.front();
		queue.pop();
		mtx.unlock();
		return temp;
	}

	size_t size() {
		mtx.lock();
		auto s = queue.size();
		mtx.unlock();
		return s;
	}
};

struct ContextPhony {
	std::mutex mtx;
};

struct ContextGuard {
	ContextGuard(ContextPhony *_ctx) : ctx(_ctx) {
		_ctx->mtx.lock();	
	}
	~ContextGuard() {
		ctx->mtx.unlock();
	}
private:
	ContextPhony *ctx;
};


class ECATSlave {
	friend class ECATMaster;
	friend class ECATSlaveDummy;
public:
	ECATSlave(ecx_context *ctx, size_t index);
	~ECATSlave() = default;

	const ec_slavet &get_slaveinfo() const {
		return ctx->slavelist[index];
	}

	void set_state(ec_state state, unsigned int timeout_ms);
	ec_state update_state();
	ec_state get_state();
	bool link_up() { return !ctx->slavelist[index].islost; }; 

private:
	void fill_from_ecx(const ec_slavet &slave);
	bool fsm_trans_valid(ec_state new_state);
	void exhaust_queue();

	enum class Action {
		SetState,
	};

	struct Context : public ContextPhony {
		ec_state state;
	};

	Context slave_context;
	ActionQueue<Action> queue;
	void execute_action(Action action);

	// I hate it, but should work, since ctx is not supposed to be null
	// until the execution stops
	ecx_context *ctx;
	const size_t index;
};
using ECATSlavePtr = std::shared_ptr<ECATSlave>;

class ECATSlaveDummy : public ECATSlave {
public:
	ECATSlaveDummy() : ECATSlave(nullptr, 0) {
		ctx = &mock_ctx;
		mock_ctx.slavelist[0].eep_man = 42;
	}

	~ECATSlaveDummy() = default;

	const ec_slavet &get_slaveinfo() const {
		return mock_ctx.slavelist[0];	
	}
private:
	ecx_context mock_ctx;
};
using ECATSlaveDummyPtr = std::shared_ptr<ECATSlaveDummy>;

class ECATMaster {
public:
	ECATMaster(const char *iface_name);
	~ECATMaster();

	void scan_bus();
	const std::vector<ECATSlavePtr>& get_slaves() const {
		return slaves;
	}

	int get_expected_wc() const { return expected_wkc; }

private:
	enum class Action {
		ConfigSlaves,
		UpdateState,
		ConfigDC,
	};

	void execute_action(Action action);

	// Stub
	struct Context : public ContextPhony {
		bool stop = false;
	};

	static const size_t IOMAP_SIZE = 4096;
	void iothread_func();
	std::thread iothread;
	Context master_context;
	ActionQueue<Action> queue;

	std::vector<std::shared_ptr<ECATSlave>> slaves;

	ecx_context ctx;
	ec_groupt *group;
	uint8 iomap[IOMAP_SIZE];
	int expected_wkc;
};
using ECATMasterPtr = std::shared_ptr<ECATMaster>;

struct BusReadError {
	const char *circumstances;
};
