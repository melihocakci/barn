#pragma once

#include "components.h"

#include <box2d/collision.h>
#include <box2d/types.h>

#include <filesystem>
#include <vector>

namespace barn {
	struct circle_def {
		b2ShapeDef shape = b2DefaultShapeDef();
		b2Circle circle{};
	};

	struct polygon_def {
		b2ShapeDef shape = b2DefaultShapeDef();
		b2Polygon polygon{};
	};

	struct body_def {
		b2BodyDef body = b2DefaultBodyDef();
		std::vector<circle_def> circles{};
		std::vector<polygon_def> polygons{};
	};

	struct sprite_def {
		std::filesystem::path texture{};
		b2Vec2 size{ 1.f, 1.f };
	};

	struct player_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::skillset skillset{};
	};

	extern const std::vector<player_def> character_templates;

	struct enemy_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::action action{};
	};

	extern const std::vector<enemy_def> enemy_templates;

	struct projectile_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
	};
}
