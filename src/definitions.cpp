#include "definitions.h"
#include "components.h"
#include "utility.h"
#include "factories.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <array>
#include <chrono>

using namespace std::chrono_literals;

const barn::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, const b2WorldId world, entt::entity player_entity) {
	},
};

///
/// Characters
///

static const barn::skill green_onion_attack
{
	.cooldown_time{ 250ms },
	.action = [](entt::registry& reg, const b2WorldId world_id, entt::entity player_entity) {
		auto [player_body, player_prop] = reg.get<barn::body, barn::properties>(player_entity);

		barn::projectile projectile_template{
			.texture{"assets/texture/green-onion.png"},
			.prop{.health = 1, .attack = player_prop.attack, .speed = 30},
			.transform{.p = b2Body_GetPosition(player_body)},
			.size{ 1.f, 1.f }
		};

		barn::projectile_factory(reg, world_id, projectile_template);
	},
};

static const barn::character hatsune_miku
{
	.texture{"assets/texture/miku.png"},
	.size{ 1.f, 2.f },
	.prop{
		.health = 1000,
		.attack = 10,
		.speed = 20,
	},
	.skills{
		green_onion_attack,
		none,
		none,
		none,
	}
};


const std::array<barn::character, 1> barn::character_templates
{
	hatsune_miku,
};


///
/// Enemies
///

static const barn::enemy pearto_enemy
{
	.texture{ "assets/texture/pearto.png" },
	.size{ 2.f, 2.f },
	.prop
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.act = [](entt::registry& reg, const b2WorldId world_id, entt::entity entity)
	{
		auto [enemy_body, enemy_stats] = reg.get<barn::body, barn::properties>(entity);
		b2Vec2 enemy_position = b2Body_GetPosition(enemy_body);

		float shortest_distance = -1.f;
		b2Vec2 closest_target{};

		for (auto [entity, _, player_body] : reg.view<barn::skillset, barn::body>().each()) {
			const b2Vec2 player_position = b2Body_GetPosition(player_body);

			float distance = length(enemy_position - player_position);

			if (distance < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance;
				closest_target = player_position;
			}
		}

		if (shortest_distance < 0) {
			b2Body_SetLinearVelocity(enemy_body, { 0,0 });
			return;
		}

		b2Vec2 vel = normalize(closest_target - enemy_position) * enemy_stats.speed;
		b2Body_SetLinearVelocity(enemy_body, vel);
	}
};


const std::array<barn::enemy, 1> barn::enemy_templates
{
	pearto_enemy,
};
