#pragma once

#include "context.hpp"
#include "levels.hpp"
#include "interface.hpp"

namespace barn {
	struct session {
		std::vector<entity_def> players{};
		barn::level level{};
	};

	barn::menu_action home_scene(barn::context& context);

	std::optional<barn::session> lobby_scene(barn::context& context);

	void combat_scene(barn::context& context, barn::session& session);
}
