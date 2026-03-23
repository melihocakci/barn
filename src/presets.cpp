#include "presets.h"
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


decltype(barn::character_presets) barn::character_presets
{
	character_preset{
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
			.duration = 1000ms,
		},
		.properties{
			.health = 1000,
			.attack = 10,
			.speed = 10,
		},
		.skillset{
			skill_def{
				.cooldown = 250ms,
				.action = [](ACTION_PARAMETERS) {
					struct local {
						barn::texture onion;
						barn::audio weiii;
					};

					if (state == ACTION_INITIALIZE) {
						registry.emplace<local>(entity,
							get_texture(renderer, textures::green_onion),
							get_audio(audios::weiii)
						);
						return;
					}
					else if (state == ACTION_CLEANUP) {
						registry.erase<local>(entity);
						return;
					}

					auto [player_body, player_prop] = registry.get<component::body, component::properties>(entity);

					local& assets = registry.get<local>(entity);
					MIX_PlayAudio(mixer, assets.weiii.get());

					barn::bullet_preset def;
					def.body.def.type = b2_kinematicBody;
					def.body.def.position = b2Body_GetPosition(player_body.id);
					def.body.def.linearVelocity = b2Vec2{ 0.f, 10.f };
					def.body.def.fixedRotation = false;
					def.body.def.angularVelocity = B2_PI;
					def.body.circles.emplace_back(ally_bullet_shape_def, b2Circle({}, 0.25f));
					def.idle_animation.texture = textures::green_onion;
					def.idle_animation.frames = { {.w = 260.f, .h = 280.f} };
					def.idle_animation.width = 1.f * PIXELS_PER_METER;
					def.properties.collide_damage = player_prop.attack;

					barn::create_bullet(FACTORY_VARIABLES, def);
				},
			},
		}
	},
};


///
/// Enemies
///


decltype(barn::enemy_presets) barn::enemy_presets
{
	enemy_preset{
		.body{
			.def = default_body_def,
			.circles{
				{foe_shape_def, b2Circle{{}, 0.5f}}
			}
		},
		.idle_animation{
			.texture = textures::pearto,
			.frames = { SDL_FRect{0.f, 0.f, 270.f, 450.f} },
			.height = 2.f * PIXELS_PER_METER,
			.duration = 1000ms,
		},
		.properties
		{
			.health = 100,
			.collide_damage = 10,
			.speed = 5,
		},
		.action = [](ACTION_PARAMETERS)
		{
			auto [enemy_body, enemy_stats] = registry.get<component::body, component::properties>(entity);
			b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

			float shortest_distance = -1.f;
			b2Vec2 closest_target{};

			for (auto [entity, _, player_body] : registry.view<component::player, component::body>().each()) {
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
