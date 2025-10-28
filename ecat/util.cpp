#include "util.hpp"

namespace EC {

std::string mb_to_str(uint16_t bitmask) {
	std::string res;
	if (bitmask & ECT_MBXPROT_COE) {
		res += "CoE ";
	}
	if (bitmask & ECT_MBXPROT_EOE) {
		res += "EoE ";
	}
	if (bitmask & ECT_MBXPROT_SOE) {
		res += "SOE ";
	}
	if (bitmask & ECT_MBXPROT_FOE) {
		res += "FoE ";
	}
	return res;
}

const char *fmmu_to_str(uint8_t func) {
	switch (func) {
		case 0:
			return "None";
		case 1:
			return "Output";
		case 2:
			return "Input";
		case 3:
			return "SM status";
		default:
			return "";
	}
}

const char *ec_state_to_str(ec_state state) {
	switch (state) {
		case EC_STATE_NONE:
			return "None";
		case EC_STATE_INIT:
			return "Init";
		case EC_STATE_PRE_OP:
			return "PreOp";
		case EC_STATE_SAFE_OP:
			return "SafeOp";
		case EC_STATE_OPERATIONAL:
			return "Op";
		case EC_STATE_BOOT:
			return "Boot";
		case EC_STATE_ACK:
			return "Ack";
		default:
			return "Invalid";
	}
}

}
