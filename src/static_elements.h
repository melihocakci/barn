#pragma once

#include "components.h"

#include <SFML/Graphics.hpp>

namespace barn {
	namespace texture {
		extern const sf::Texture miku;
		extern const sf::Texture pearto;
		extern const sf::Texture green_onion;
		extern const sf::Texture bliss;
	}

	struct character {
		barn::sprite sprite;
		b2Vec2 size;
		barn::properties properties;
		barn::skillset skillset;
	};

	extern const std::array<character, 1> character_templates;

	struct enemy {
		barn::sprite sprite;
		b2Vec2 size;
		barn::properties stats;
		barn::action action;
	};

	extern const std::array<enemy, 1> enemy_templates;
}
