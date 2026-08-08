#pragma once

#include "context.h"
#include "levels.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	enum class menu_state {
		MAIN_MENU,
		SETTINGS_MENU,
		PAUSE_MENU,
		START_GAME,
		EXIT,
	};

	struct session {
		std::vector<entity_def> players{};
		barn::level level{};
	};

	barn::menu_state home_scene(barn::context& context);

	std::optional<barn::session> lobby_scene(barn::context& context);

	void combat_scene(barn::context& context, barn::session& session);
}
