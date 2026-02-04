#pragma once

#include "components.h"

#include <SFML/Graphics.hpp>

namespace project_stable {
	struct character {
		sprite sprite;
		hitbox hitbox;
		stats stats;
		skillset skillset;
	};

	extern const std::array<character, 1> character_templates;
}
