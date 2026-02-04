#include "definitions.h"
#include "components.h"
#include "utils.h"
#include "factories.h"
#include "constants.h"

#include <box2d/box2d.h>

#include <chrono>

using namespace std::chrono_literals;

static const b2BodyDef default_body_def = [] {
	b2BodyDef def = b2DefaultBodyDef();
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	return def;
	}();

static const b2ShapeDef ally_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.enableContactEvents = true;
	def.filter.categoryBits = barn::category::ALLY;
	def.filter.maskBits = barn::category::FOE | barn::category::FOE_BULLET | barn::category::OBSTACLE;
	return def;
	}();

static const b2ShapeDef foe_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.enableContactEvents = true;
	def.filter.categoryBits = barn::category::FOE;
	def.filter.maskBits = barn::category::ALLY | barn::category::ALLY_BULLET | barn::category::OBSTACLE;
	return def;
	}();

static const b2ShapeDef ally_bullet_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.filter.categoryBits = barn::category::ALLY_BULLET;
	def.filter.maskBits = barn::category::FOE | barn::category::OBSTACLE;
	return def;
	}();

static const b2ShapeDef foe_bullet_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.filter.categoryBits = barn::category::FOE_BULLET;
	def.filter.maskBits = barn::category::ALLY | barn::category::OBSTACLE;
	return def;
	}();


///
/// Characters
///


decltype(barn::character_templates) barn::character_templates
{
	player_def{
		.body{
			.body = default_body_def,
			.circles{
				{ally_shape_def, b2Circle{{}, 0.25f}}
			}
		},
		.sprite{
			.texture = "texture/miku.png",
			//.width = 1.f * PIXELS_PER_METER,
			.height = 1.58f * PIXELS_PER_METER,
		},
		.properties{
			.health = 1000,
			.attack = 10,
			.speed = 10,
		},
		.skillset{
			.skill1 {
				.cooldown = 250ms,
				.action = [](entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, entt::entity player_entity) {
					auto [player_body, player_prop] = reg.get<barn::body, barn::properties>(player_entity);

					barn::bullet_def def;
					def.body.body.type = b2_kinematicBody;
					def.body.body.position = b2Body_GetPosition(player_body.id);
					def.body.body.linearVelocity = b2Vec2{ 0.f, 20.f };
					def.body.circles.emplace_back(ally_bullet_shape_def, b2Circle({}, 0.25f));
					def.sprite.texture = "texture/green-onion.png";
					def.sprite.width = 1.f * PIXELS_PER_METER;
					def.properties.attack = player_prop.attack;
					def.type = barn::category::ALLY_BULLET;

					barn::create_bullet(reg, renderer, world_id, def);
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
			.body = default_body_def,
			.circles{
				{foe_shape_def, b2Circle{{}, 0.5f}}
			}
		},
		.sprite{
			.texture = "texture/pearto.png",
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
				b2Body_SetLinearVelocity(enemy_body.id, { 0, 0 });
				return;
			}

			b2Vec2 vel = normalize(closest_target - enemy_position) * enemy_stats.speed;
			b2Body_SetLinearVelocity(enemy_body.id, vel);
		}
	},
};
