#pragma once

#include "context.hpp"

namespace barn {
	enum class menu {
		MAIN_MENU,
		SETTINGS_MENU,
		PAUSE_MENU,
	};

	enum class menu_action {
		NONE,
		START_GAME,
		EXIT,
	};

	barn::menu_action draw_menu(barn::context& context, std::vector<barn::menu>& menu_stack);

	void draw_ui(barn::context& context, entt::registry& registry);
}
