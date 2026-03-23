#pragma once

#include "components.h"

namespace barn::config {
	struct keyboard_controls {
		SDL_Scancode up{};
		SDL_Scancode down{};
		SDL_Scancode left{};
		SDL_Scancode right{};
		SDL_Scancode skill1{};
		SDL_Scancode skill2{};
		SDL_Scancode skill3{};
		SDL_Scancode skill4{};
	};

	struct gamepad_controls {
		SDL_GamepadAxis axis_x{};
		SDL_GamepadAxis axis_y{};
		SDL_GamepadButton skill1{};
		SDL_GamepadButton skill2{};
		SDL_GamepadButton skill3{};
		SDL_GamepadButton skill4{};
	};

	extern std::array<keyboard_controls, static_cast<int>(component::player::COUNT)> keyboard_bindings;
	extern std::array<gamepad_controls, static_cast<int>(component::player::COUNT)> gamepad_bindings;
}
