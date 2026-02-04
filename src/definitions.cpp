#include "definitions.h"
#include "components.h"
#include "utils.h"
#include "factories.h"
#include "constants.h"

#include <box2d/box2d.h>

#include <chrono>

using namespace std::chrono_literals;

static const b2BodyDef dynamic_body_def = [] {
	b2BodyDef def = b2DefaultBodyDef();
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	return def;
	}();

///
/// Characters
///


decltype(barn::character_templates) barn::character_templates
{
	player_def{
		.body{
			.body = dynamic_body_def,
			.circles{
				{.circle{ {}, 0.5f }}
			}
		},
		.sprite{
			.texture = "texture/miku.png",
			.width = 1.f * PIXELS_PER_METER,
			.height = 2.f * PIXELS_PER_METER,
		},
		.properties{
			.health = 1000,
			.attack = 10,
			.speed = 20,
		},
		.skillset{
			.skill1 {
				.cooldown_time{ 250ms },
				.action = [](entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, entt::entity player_entity) {
					auto [player_body, player_prop] = reg.get<barn::body, barn::properties>(player_entity);

					barn::projectile_def projdef;
					projdef.body.body.type = b2_kinematicBody;
					projdef.body.body.isBullet = true;
					projdef.body.body.position = b2Body_GetPosition(player_body.id);
					projdef.body.body.linearVelocity = b2Vec2{ 0.f, player_prop.speed };
					projdef.body.circles.emplace_back(b2DefaultShapeDef(), b2Circle({}, 0.5f));
					projdef.sprite.texture = "texture/green-onion.png";
					//projdef.sprite.width = 1.f * PIXELS_PER_METER;
					//projdef.sprite.height = 1.f * PIXELS_PER_METER;
					projdef.properties.attack = player_prop.attack;

					barn::create_projectile(reg, renderer, world_id, projdef);
				},
			},
		}
	},
};


///
/// Enemies
///


decltype(barn::enemy_templates) barn::enemy_templates
{
	enemy_def{
		.body{
			.body = dynamic_body_def,
			.circles{
				{.circle{{ 0, 0 }, 0.5f }}
			}
		},
		.sprite{
			.texture = "texture/pearto.png",
			.width = 2.f * PIXELS_PER_METER,
			.height = 2.f * PIXELS_PER_METER,
		},
		.properties
		{
			.health = 100,
			.attack = 10,
			.speed = 5,
		},
		.action = [](entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, entt::entity entity)
		{
			auto [enemy_body, enemy_stats] = reg.get<barn::body, barn::properties>(entity);
			b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

			float shortest_distance = -1.f;
			b2Vec2 closest_target{};

			for (auto [entity, _, player_body] : reg.view<barn::skillset, barn::body>().each()) {
				const b2Vec2 player_position = b2Body_GetPosition(player_body.id);

				float distance = length(enemy_position - player_position);

				if (distance < shortest_distance || shortest_distance < 0)
				{
					shortest_distance = distance;
					closest_target = player_position;
				}
			}

			if (shortest_distance < 0) {
				b2Body_SetLinearVelocity(enemy_body.id, { 0,0 });
				return;
			}

			b2Vec2 vel = normalize(closest_target - enemy_position) * enemy_stats.speed;
			b2Body_SetLinearVelocity(enemy_body.id, vel);
		}
	},
};
