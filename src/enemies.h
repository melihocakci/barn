#pragma once

#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace mg {
	struct enemy {
		sprite sprite;
		hitbox hitbox;
		stats stats;
		action action;
	};

	extern const std::array<enemy, 1> enemy_templates;
}
