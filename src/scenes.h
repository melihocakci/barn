#pragma once

#include "context.h"
#include "levels.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	struct session {
		std::vector<entity_def> players{};
		barn::level level{};
	};

	int main_menu(barn::context& context);

	int combat_scene(barn::context& context, barn::session& session);
}
