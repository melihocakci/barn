#include "presets.hpp"
#include "components.hpp"
#include "utils.hpp"
#include "factories.hpp"
#include "constants.hpp"
#include "assets.hpp"
#include "skills.hpp"
#include "ai_behaviors.hpp"

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
	def.filter.maskBits = barn::category::ENEMY | barn::category::ENEMY_BULLET | barn::category::OBSTACLE;
	return def;
	}();

static const b2ShapeDef enemy_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.enableContactEvents = true;
	def.filter.categoryBits = barn::category::ENEMY;
	def.filter.maskBits = barn::category::ALLY | barn::category::ALLY_BULLET | barn::category::OBSTACLE;
	return def;
	}();

///
/// Characters
///


decltype(barn::character_presets) barn::character_presets
{
	entity_def{
		.body = body_def{
			.def = default_body_def,
			.circles{
				{ally_shape_def, b2Circle{{}, 0.25f}}
			}
		},
		.animation_list = barn::animation_list_def{
			.idle_animation = barn::animation_def{
				.texture = textures::miku_animation,
				.frames = []() -> std::vector<SDL_FRect> {
					std::vector<SDL_FRect> rects;
					for (int i = 0; i < 20; ++i) {
						rects.emplace_back(i * 59.f, 0.f, 59.f, 64.f);
					}
					return rects;
				}(),
				.width = 1.2f * PIXELS_PER_METER,
				.height = .8f * PIXELS_PER_METER,
				.duration = 2000ms,
			},
			.attack_animation = barn::animation_def{
				.texture = textures::miku_animation,
				.frames = []() -> std::vector<SDL_FRect> {
					std::vector<SDL_FRect> rects;
					for (int i = 0; i < 8; ++i) {
						rects.emplace_back(i * 85.f, 236.f, 85.f, 64.f);
					}
					return rects;
				}(),
				.height = .8f * PIXELS_PER_METER,
				.duration = 500ms,
			}
		},
		.base_properties = component::base_properties{
			.health = 1000,
			.attack = 10,
			.speed = 10,
		},
		.skillset = component::skillset{
			skills::green_onion{},
		}
	},
};


///
/// Enemies
///


decltype(barn::enemy_presets) barn::enemy_presets
{
	entity_def{
		.body = body_def{
			.def = default_body_def,
			.circles{
				{enemy_shape_def, b2Circle{{}, 0.5f}}
			}
		},
		.animation = animation_def{
			.texture = textures::pearto,
			.frames = { SDL_FRect{0.f, 0.f, 270.f, 450.f} },
			.height = 2.f * PIXELS_PER_METER,
			.duration = 1000ms,
		},
		.base_properties = component::base_properties{
			.health = 100,
			.collide_damage = 10,
			.speed = 5,
		},
		.ai_behavior = barn::ai_behaviors::chaser{},
	},
};
