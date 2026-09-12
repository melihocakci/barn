#pragma once

#include "context.hpp"
#include "components.hpp"

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

	enum category : std::uint64_t {
		ALLY = 1,
		ENEMY = 1 << 1,
		ALLY_BULLET = 1 << 2,
		ENEMY_BULLET = 1 << 3,
		OBSTACLE = 1 << 4,
	};

	struct base_properties {
		int health = 1;
		int attack = 0;
		int defense = 0;
		int collide_damage = 0;
		int speed = 1;
	};

	struct sprite_def {
		barn::asset_def texture{};
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	using namespace std::chrono_literals;

	struct animation_def {
		barn::asset_def texture{};
		std::vector<SDL_FRect> frames{};
		std::optional<float> width{};
		std::optional<float> height{};
		std::chrono::milliseconds duration = 1000ms;
	};

	struct track_def {
		barn::asset_def audio{};
		SDL_PropertiesID properties_id{};
	};

	struct assets_def {
		std::vector<barn::asset_def> textures{};
		std::vector<barn::asset_def> audios{};
	};

	struct skill_def {
		barn::skill_code code{};
		barn::assets_def assets{};
		std::chrono::milliseconds cooldown{};
	};

	using skillset_def = std::array<skill_def, SKILLSET_SIZE>;

	struct entity_def {
		std::optional<barn::body_def> body{};
		std::optional<barn::animation_def> idle_animation{};
		std::optional<barn::sprite_def> sprite{};
		std::optional<barn::track_def> track{};
		std::optional<barn::base_properties> properties{};
		std::optional<barn::skillset_def> skillset{};
		std::optional<component::keyboard> keyboard{};
		std::optional<component::gamepad> gamepad{};
		std::optional<component::transform> transform{};
		std::optional<component::AI_code> AI_code{};
		std::optional<component::player> player{};
		std::optional<component::enemy> enemy{};
		std::optional<component::bullet> bullet{};
		std::optional<component::obstacle> obstacle{};
		std::optional<component::background> background{};
	};

	entt::entity create_entity(entt::registry& registry, barn::context& context, const barn::entity_def& def);
}
