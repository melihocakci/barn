#pragma once

#include "components.h"

#include <SDL3/SDL.h>
#include <glaze/json.hpp>

namespace barn {
	struct keyboard_controls {
		SDL_Scancode up{ SDL_SCANCODE_UP };
		SDL_Scancode down{ SDL_SCANCODE_DOWN };
		SDL_Scancode left{ SDL_SCANCODE_LEFT };
		SDL_Scancode right{ SDL_SCANCODE_RIGHT };
		std::array<SDL_Scancode, SKILLSET_SIZE> skills{
			SDL_SCANCODE_Q,
			SDL_SCANCODE_W,
			SDL_SCANCODE_E,
			SDL_SCANCODE_R,
		};
	};

	struct gamepad_controls {
		SDL_GamepadAxis axis_x{ SDL_GAMEPAD_AXIS_LEFTX };
		SDL_GamepadAxis axis_y{ SDL_GAMEPAD_AXIS_LEFTY };
		std::array<SDL_GamepadButton, SKILLSET_SIZE> skills{
			SDL_GAMEPAD_BUTTON_WEST,
			SDL_GAMEPAD_BUTTON_EAST,
			SDL_GAMEPAD_BUTTON_SOUTH,
			SDL_GAMEPAD_BUTTON_NORTH,
		};
	};

	struct settings {
		std::array<keyboard_controls, PLAYER_COUNT> keyboard_bindings{};
		std::array<gamepad_controls, PLAYER_COUNT> gamepad_bindings{};
		float master_volume = 100.f;
		bool show_fps = false;
	};

	glz::error_ctx load_settings(barn::settings& settings);

	glz::error_ctx save_settings(const barn::settings& settings);

	void apply_settings(const barn::settings& settings, SDL_Renderer* renderer, MIX_Mixer* mixer);
}
