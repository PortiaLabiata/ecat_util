#include "util.hpp"

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

std::string fmmu_to_str(uint8_t func) {
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
