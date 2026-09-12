#include "skills.hpp"
#include "components.hpp"
#include "context.hpp"
#include "assets.hpp"
#include "factories.hpp"

static const b2BodyDef default_body_def = [] {
	b2BodyDef def = b2DefaultBodyDef();
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	return def;
	}();

static const b2ShapeDef bullet_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.filter.categoryBits = barn::category::ALLY_BULLET;
	def.filter.maskBits = barn::category::ENEMY | barn::category::OBSTACLE;
	return def;
	}();

void barn::skills::green_onion::initialize(barn::context& context) {
	// Initialization logic for the green onion skill
}

void barn::skills::green_onion::key_down(barn::context& context, entt::registry& registry, entt::entity entity) {
	auto [player_body, player_prop] = registry.get<barn::component::body, barn::component::properties>(entity);

	MIX_PlayAudio(context.mixer, barn::get_audio(barn::audios::weiii).get());

	b2BodyDef body_def = default_body_def;
	body_def.type = b2_kinematicBody;
	body_def.position = b2Body_GetPosition(player_body.id);
	body_def.linearVelocity = { 0.f, 10.f };
	body_def.angularVelocity = B2_PI;

	barn::entity_def def{
		.body = barn::body_def{
			.def = body_def,
			.circles{
				{bullet_shape_def, b2Circle{{}, 0.25f}}
			}
		},
		.idle_animation = barn::animation_def{
			.texture = barn::textures::green_onion,
			.frames = { SDL_FRect{0.f, 0.f, 260.f, 280.f} },
			.width = 1.f * barn::PIXELS_PER_METER,
		},
		.properties = barn::base_properties{
			.collide_damage = player_prop.attack,
		},
		.bullet = barn::component::bullet{}
	};

	barn::create_entity(registry, context, def);

	start_cooldown(registry, entity);
}

void barn::skills::green_onion::key_up(barn::context& context, entt::registry& registry, entt::entity entity) {
	// Logic for when the key is released
}
