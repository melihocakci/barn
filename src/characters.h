#pragma once

#include "components.h"

#include <SFML/Graphics.hpp>

namespace mg {
	struct character {
		std::string name;
		sprite sprite;
		skillset skillset;
	};

	extern const std::array<character, 1> playable_characters;
}
