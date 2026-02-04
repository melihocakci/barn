#pragma once

#include "components.h"

namespace barn::config {
	extern std::array<keyboard_controls, player::COUNT> keyboard_bindings;
	extern std::array<gamepad_controls, player::COUNT> gamepad_bindings;
}
