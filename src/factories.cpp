#include "factories.h"
#include "components.h"
#include "constants.h"
#include "utils.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

static void add_body(entt::entity entity, entt::registry& reg, b2WorldId world_id, const barn::body_def& def) {
	const b2BodyId body_id = b2CreateBody(world_id, &def.def);

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body_id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body_id, &shape_def, &polygon);
	}

	b2Body_SetUserData(body_id, new entt::entity{ entity });

	reg.emplace<barn::body>(entity, body_id);
}

static void add_sprite(entt::entity entity, entt::registry& reg, SDL_Renderer* renderer, const barn::sprite_def& def) {
	reg.emplace<barn::sprite>(entity, barn::get_texture(renderer, def.texture), def.src_rect, def.width, def.height);
}

entt::entity barn::create_borders(entt::registry& reg, b2WorldId world_id) {
	entt::entity entity = reg.create();

	barn::body_def body_def{};
	body_def.def.type = b2_staticBody;
	body_def.def.position = { 0, 0 };

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.categoryBits = barn::category::OBSTACLE;

	constexpr float half_width = 2.f;

	b2Polygon top_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS + half_width }, b2Rot_identity);
	b2Polygon bottom_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, -half_width }, b2Rot_identity);
	b2Polygon left_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { -half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2Polygon right_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { VIRTUAL_WIDTH_METERS + half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);

	body_def.polygons = {
		{ shape_def, top_rect },
		{ shape_def, bottom_rect },
		{ shape_def, left_rect },
		{ shape_def, right_rect }
	};

	add_body(entity, reg, world_id, body_def);

	reg.emplace<barn::category>(entity, barn::category::OBSTACLE);

	return entity;
}

entt::entity barn::create_player(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::player_def& def) {
	const entt::entity entity = reg.create();

	add_sprite(entity, reg, renderer, def.sprite);

	add_body(entity, reg, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::skillset>(entity, def.skillset);

	reg.emplace<barn::category>(entity, barn::category::ALLY);

	return entity;
}

entt::entity barn::create_enemy(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::enemy_def& def) {
	const entt::entity entity = reg.create();

	add_sprite(entity, reg, renderer, def.sprite);

	add_body(entity, reg, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::action>(entity, def.action);

	reg.emplace<barn::category>(entity, barn::category::FOE);

	return entity;
}

entt::entity barn::create_bullet(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::bullet_def& def) {
	const entt::entity entity = reg.create();

	add_sprite(entity, reg, renderer, def.sprite);

	add_body(entity, reg, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::category>(entity, def.type);

	reg.emplace<barn::bullet>(entity);

	return entity;
}
