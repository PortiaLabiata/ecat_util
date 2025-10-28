#pragma once
#include <inttypes.h>
#include <string>
#include <soem/soem.h>

namespace EC {

std::string mb_to_str(uint16_t bitmask);
const char *fmmu_to_str(uint8_t func);
const char *ec_state_to_str(ec_state state);

}
