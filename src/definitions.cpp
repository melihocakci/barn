#include "definitions.h"
#include "components.h"
#include "utils.h"
#include "factories.h"
#include "constants.h"
#include "assets.h"

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
			.def = default_body_def,
			.circles{
				{ally_shape_def, b2Circle{{}, 0.25f}}
			}
		},
		.idle_animation{
			.texture = textures::miku_animation,
			.frames = []() -> std::vector<SDL_FRect> {
				std::vector<SDL_FRect> rects;
				for (int i = 0; i < 20; ++i) {
					rects.push_back({ i * 59.f, 0.f, 59.f, 64.f });
				}
				return rects;
			}(),
			.height = .8f * PIXELS_PER_METER,
			.frame_duration = 100ms,
			.loop_count = -1,
		},
		.properties{
			.health = 1000,
			.attack = 10,
			.speed = 10,
		},
		.assets{
			.textures = {
				textures::green_onion,
			},
			.audios = {
				audios::weiii,
			},
		},
		.skillset{
			skill{
				.cooldown = 250ms,
				.action = [](ACTION_PARAMETERS) {
					auto [player_body, player_prop] = registry.get<barn::body, barn::properties>(entity);

					barn::audio& weiii_sound = registry.get<barn::assets>(entity).audios[0];
					MIX_PlayAudio(mixer, weiii_sound.get());

					barn::bullet_def def;
					def.body.def.type = b2_kinematicBody;
					def.body.def.position = b2Body_GetPosition(player_body.id);
					def.body.def.linearVelocity = b2Vec2{ 0.f, 20.f };
					def.body.circles.emplace_back(ally_bullet_shape_def, b2Circle({}, 0.25f));
					def.sprite.texture = registry.get<barn::assets>(entity).textures[0];
					def.sprite.width = 1.f * PIXELS_PER_METER;
					def.properties.attack = player_prop.attack;
					def.type = barn::category::ALLY_BULLET;

					barn::create_bullet(registry, renderer, world, def);
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
			.def = default_body_def,
			.circles{
				{foe_shape_def, b2Circle{{}, 0.5f}}
			}
		},
		.sprite{
			.texture = textures::pearto,
			.height = 2.f * PIXELS_PER_METER,
		},
		.properties
		{
			.health = 100,
			.attack = 10,
			.speed = 5,
		},
		.action = [](ACTION_PARAMETERS)
		{
			auto [enemy_body, enemy_stats] = registry.get<barn::body, barn::properties>(entity);
			b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

			float shortest_distance = -1.f;
			b2Vec2 closest_target{};

			for (auto [entity, _, player_body] : registry.view<barn::player, barn::body>().each()) {
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
