#pragma once

#include "context.h"
#include "levels.h"
#include "interface.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	struct session {
		std::vector<entity_def> players{};
		barn::level level{};
	};

	barn::menu_state home_scene(barn::context& context);

	std::optional<barn::session> lobby_scene(barn::context& context);

	void combat_scene(barn::context& context, barn::session& session);
}
