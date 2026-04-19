#pragma once

#include "components.h"

namespace barn::config {
	struct keyboard_controls {
		SDL_Scancode up{};
		SDL_Scancode down{};
		SDL_Scancode left{};
		SDL_Scancode right{};
		SDL_Scancode skills[barn::SKILLSET_SIZE]{};
	};

	struct gamepad_controls {
		SDL_GamepadAxis axis_x{};
		SDL_GamepadAxis axis_y{};
		SDL_GamepadButton skills[barn::SKILLSET_SIZE]{};
	};

	extern std::array<keyboard_controls, static_cast<int>(component::player::COUNT)> keyboard_bindings;
	extern std::array<gamepad_controls, static_cast<int>(component::player::COUNT)> gamepad_bindings;
}
