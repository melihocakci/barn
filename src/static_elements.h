#pragma once

#include "components.h"

namespace mg {
	extern const skill basic_attack;

	struct character {
		std::string name;
		const sf::Texture texture;
		int health;
		int mana;
		int attack;
		int defense;
		int speed;
		skillset skillset;
	};

	extern const std::array<character, 1> playable_characters;
}
