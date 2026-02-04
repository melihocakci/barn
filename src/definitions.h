#pragma once

#include "components.h"

#include <array>
#include <filesystem>

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
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	struct player_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::skillset skillset{};
	};

	struct enemy_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::action action{};
	};

	struct projectile_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
	};

	extern const std::array<player_def, 1> character_templates;

	extern const std::array<enemy_def, 1> enemy_templates;
}
