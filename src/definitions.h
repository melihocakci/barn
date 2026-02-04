#pragma once

#include "components.h"

#include <SFML/Graphics.hpp>
#include <box2d/types.h>

#include <filesystem>

namespace barn {
	struct character {
		std::filesystem::path texture;
		b2Vec2 size;
		properties prop;
		skillset skills;
	};

	extern const std::array<character, 1> character_templates;

	struct enemy {
		std::filesystem::path texture;
		b2Vec2 size;
		properties prop;
		action act;
	};

	extern const std::array<enemy, 1> enemy_templates;

	struct projectile {
		std::filesystem::path texture;
		properties prop;
		b2Transform transform;
		b2Vec2 size;
		b2Vec2 velocity;
	};
}
