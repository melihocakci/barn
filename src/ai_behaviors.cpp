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

static const b2ShapeDef bullet_shape_def = [] {
	b2ShapeDef def = b2DefaultShapeDef();
	def.filter.categoryBits = barn::category::ALLY_BULLET;
	def.filter.maskBits = barn::category::ENEMY | barn::category::OBSTACLE;
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
