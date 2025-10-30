#pragma once
#include <linux/rtnetlink.h>
#include <ifaddrs.h>

class NetStat {
public:
	NetStat() {
	}

private:
	struct ifaddr *iface;
};
