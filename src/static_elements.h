#pragma once

#include "components.h"

#include <SFML/Graphics.hpp>

namespace project_stable {
	namespace texture {
		extern const sf::Texture miku;
		extern const sf::Texture pearto;
		extern const sf::Texture green_onion;
		extern const sf::Texture bliss;
	}

	struct character {
		sprite sprite;
		hitbox hitbox;
		properties stats;
		skillset skillset;
	};

	extern const std::array<character, 1> character_templates;

	struct enemy {
		sprite sprite;
		hitbox hitbox;
		properties stats;
		action action;
	};

	extern const std::array<enemy, 1> enemy_templates;
}
