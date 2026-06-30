#pragma once

#include "settings.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>

namespace barn {
	struct context {
		SDL_Window* const window{};
		SDL_Renderer* const renderer{};
		MIX_Mixer* const mixer{};
		b2WorldId const world_id{};
		barn::settings settings{};
        std::vector<barn::gamepad> gamepads{};
	};
}
