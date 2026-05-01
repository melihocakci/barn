#pragma once

#include "components.h"

namespace barn {
	struct game_settings {
		float master_volume = 100.f;
	};

	struct context {
		SDL_Window* const window{};
		SDL_Renderer* const renderer{};
		MIX_Mixer* const mixer{};
		b2WorldId const world_id{};
		barn::game_settings settings{};
	};
}
