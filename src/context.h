#pragma once

#include "settings.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>

#include <unordered_map>

namespace barn {
	enum class game_state {
		COMBAT,
		MENU,
		OPTIONS,
		EXIT,
	};

	struct context {
		SDL_Window* const window{};
		SDL_Renderer* const renderer{};
		MIX_Mixer* const mixer{};
		b2WorldId const world_id{};
		barn::settings settings{};
        std::unordered_map<SDL_JoystickID, barn::gamepad> gamepads{};
		barn::game_state state{};
	};
}
