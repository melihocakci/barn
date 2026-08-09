#pragma once

#include "context.h"

namespace barn {
	enum class menu_state {
		MAIN_MENU,
		SETTINGS_MENU,
		PAUSE_MENU,
		START_GAME,
		EXIT,
	};

	void draw_menu(barn::context& context, std::vector<barn::menu_state>& menu_stack);

	void draw_ui(barn::context& context, entt::registry& registry);
}
