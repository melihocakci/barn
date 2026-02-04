#pragma once

#include "components.h"

#include <array>

constexpr barn::action empty_action = [](entt::entity, entt::registry&, SDL_Renderer*, MIX_Mixer*, b2WorldId) {};

namespace barn {
	struct circle_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Circle circle{};
	};

	struct polygon_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Polygon polygon{};
	};

	struct body_def {
		b2BodyDef def = b2DefaultBodyDef();
		std::vector<circle_def> circles{};
		std::vector<polygon_def> polygons{};
	};

	using texture_def = std::string_view;

	using audio_def = std::string_view;

	struct assets_def {
		std::vector<barn::texture_def> textures{};
		std::vector<barn::audio_def> audios{};
	};

	struct sprite_def {
		barn::texture_def texture{};
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	struct skill_def {
		std::chrono::milliseconds cooldown{};
		std::vector<barn::texture_def> textures{};
		std::vector<barn::audio_def> audios{};
		barn::action action = empty_action;
	};

	using skillset_def = std::array<barn::skill_def, barn::skillset_size>;

	struct player_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::assets_def assets{};
		barn::skillset_def skillset{};
	};

	struct enemy_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::action action = empty_action;
	};

	struct bullet_def {
		barn::body_def body{};
		barn::sprite_def sprite{};
		barn::properties properties{};
		barn::category type{};
	};

	extern const std::array<player_def, 1> character_templates;

	extern const std::array<enemy_def, 1> enemy_templates;
}
