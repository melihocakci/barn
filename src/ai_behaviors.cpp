#include "ai_behaviors.hpp"
#include "components.hpp"
#include "context.hpp"
#include "assets.hpp"
#include "factories.hpp"
#include "utils.hpp"

static const b2BodyDef default_body_def = [] {
	b2BodyDef def = b2DefaultBodyDef();
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	return def;
	}();

static const b2ShapeDef enemy_bullet_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.filter.categoryBits = barn::category::ENEMY_BULLET;
	def.filter.maskBits = barn::category::ALLY | barn::category::OBSTACLE;
	return def;
	}();

void barn::ai_behaviors::chaser::update(barn::context& context, entt::registry& registry, entt::entity entity) {
	auto [enemy_body, enemy_stats] = registry.get<component::body, component::properties>(entity);
	b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

	float shortest_distance = -1.f;
	b2Vec2 closest_target{};

	for (auto [player_entity, player, player_body] : registry.view<component::player, component::body>().each()) {
		const b2Vec2 player_position = b2Body_GetPosition(player_body.id);

		float distance = barn::length(enemy_position - player_position);

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

	b2Vec2 vel = barn::normalize(closest_target - enemy_position) * static_cast<float>(enemy_stats.speed);
	b2Body_SetLinearVelocity(enemy_body.id, vel);
}

void barn::ai_behaviors::pearto::initialize(barn::context& context) {
	this->baguette = barn::get_texture(context, barn::textures::baguette);
}

void barn::ai_behaviors::pearto::update(barn::context& context, entt::registry& registry, entt::entity entity) {
	if (on_cooldown()) return;

	auto [enemy_body, enemy_prop] = registry.get<component::body, component::properties>(entity);
	const b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

	static constexpr int PEARTO_ARM_COUNT = 3;
	static constexpr float PEARTO_BULLET_SPEED = 4.f;
	// Positive step applied every shot so the arms always wind the same way (counter-clockwise), never alternating.
	static constexpr float PEARTO_SPIRAL_ROTATION_STEP = B2_PI / 15.f;

	for (int i = 0; i < PEARTO_ARM_COUNT; ++i) {
		const float angle = rotation_offset + i * (2.f * B2_PI / PEARTO_ARM_COUNT);
		const b2Vec2 direction{ cosf(angle), sinf(angle) };

		b2BodyDef body_def = default_body_def;
		body_def.type = b2_kinematicBody;
		body_def.position = enemy_position;
		body_def.linearVelocity = direction * PEARTO_BULLET_SPEED;
		body_def.angularVelocity = B2_PI;

		barn::entity_def def{
			.body = barn::body_def{
				.def = body_def,
				.circles{
					{enemy_bullet_shape_def, b2Circle{{}, 0.2f}}
				}
			},
			.sprite = barn::sprite_def{
				.texture = barn::textures::baguette,
				.width = 0.5f * barn::PIXELS_PER_METER,
				.height = 0.5f * barn::PIXELS_PER_METER,
			},
			.base_properties = component::base_properties{
				.collide_damage = enemy_prop.attack,
			},
			.bullet = barn::component::bullet{}
		};

		barn::create_entity(registry, context, def);
	}

	rotation_offset += PEARTO_SPIRAL_ROTATION_STEP;

	start_cooldown(registry, entity);
}
