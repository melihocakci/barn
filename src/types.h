#pragma once

#include "components.h"

namespace barn {
	struct context {
		SDL_Window* const window{};
		SDL_Renderer* const renderer{};
		MIX_Mixer* const mixer{};
		b2WorldId const world_id{};
	};
}
