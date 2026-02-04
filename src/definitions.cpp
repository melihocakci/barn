#include "definitions.h"
#include "components.h"
#include "utility.h"
#include "factories.h"

#include <box2d/box2d.h>

#include <array>
#include <chrono>
#include <box2d/types.h>

using namespace std::chrono_literals;

///
/// Characters
///

static const barn::skill green_onion_attack
{
	.cooldown_time{ 250ms },
	.action = [](entt::registry& reg, const b2WorldId world_id, entt::entity player_entity) {
		auto [player_body, player_prop] = reg.get<barn::body, barn::properties>(player_entity);

		barn::projectile_def projdef;
		projdef.body.body.type = b2_dynamicBody;
		projdef.body.body.position = b2Body_GetPosition(player_body);
		b2Vec2 forward = b2Vec2{ 0.f, 1.f };
		projdef.body.body.linearVelocity = forward * player_prop.speed;
		projdef.body.polygons.emplace_back(b2DefaultShapeDef(), b2MakeBox(0.2f, 0.5f));
		projdef.sprite.texture = "assets/texture/green-onion.png";
		projdef.sprite.size = { 0.4f, 1.f };
		projdef.properties.attack = player_prop.attack;

		barn::create_projectile(reg, world_id, projdef);
	},
};

static const barn::player_def hatsune_miku
{
	.body{
		.body = [] {
			b2BodyDef def = b2DefaultBodyDef();
			def.type = b2_dynamicBody;
			return def;
		}(),
		.polygons{
			{.polygon = b2MakeBox(0.25f, 0.5f)}
		}
	},
	.sprite{
		.texture = "assets/texture/miku.png",
		.size = {0.5f, 1.f}
	},
	.properties{
		.health = 1000,
		.attack = 10,
		.speed = 20,
	},
	.skillset{
		.skill_1 = green_onion_attack,
	}
};


const std::vector<barn::player_def> barn::character_templates
{
	hatsune_miku,
};


///
/// Enemies
///

static const barn::enemy_def pearto_enemy
{
	.body{
		.body = [] {
			b2BodyDef def = b2DefaultBodyDef();
			def.type = b2_dynamicBody;
			return def;
		}(),
		.polygons{
			{.polygon = b2MakeBox(0.25f, 0.5f)}
		}
	},
	.sprite{
		.texture = "assets/texture/pearto.png",
		.size = {1.f, 1.f}
	},
	.properties
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.action = [](entt::registry& reg, const b2WorldId world_id, entt::entity entity)
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

const std::vector<barn::enemy_def> barn::enemy_templates
{
	pearto_enemy,
};
