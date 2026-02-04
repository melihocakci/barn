#include "factories.h"
#include "components.h"
#include "constants.h"
#include "utils.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

static barn::body make_body(b2WorldId world_id, const barn::body_def& def) {
	b2BodyId body_id = b2CreateBody(world_id, &def.body);

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body_id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body_id, &shape_def, &polygon);
	}

	return body_id;
}

static barn::sprite make_sprite(SDL_Renderer* renderer, const barn::sprite_def& def) {
	return { barn::get_texture(renderer, def.texture), def.src_rect, def.width, def.height };
}

entt::entity barn::create_borders(entt::registry& reg, b2WorldId world_id) {
	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_staticBody;
	body_def.position = { 0, 0 };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 0.f;
	shape_def.material.friction = 0.3f;

	constexpr float half_width = 2.f;

	b2Polygon top_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS + half_width }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &top_rect);

	b2Polygon bottom_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, -half_width }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &bottom_rect);

	b2Polygon left_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { -half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &left_rect);

	b2Polygon right_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { VIRTUAL_WIDTH_METERS + half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &right_rect);

	entt::entity entity = reg.create();
	reg.emplace<barn::body>(entity, body_id);
	reg.emplace<barn::type>(entity, barn::type::OBSTACLE);
	reg.emplace<barn::alignment>(entity, barn::alignment::NEUTRAL);
	reg.emplace<barn::properties>(entity);

	return entity;
}

entt::entity barn::create_player(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::player_def& def) {
	const entt::entity entity = reg.create();

	reg.emplace<barn::sprite>(entity, make_sprite(renderer, def.sprite));

	reg.emplace<barn::body>(entity, make_body(world_id, def.body));

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::skillset>(entity, def.skillset);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	return entity;
}

entt::entity barn::create_enemy(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::enemy_def& def) {
	const entt::entity entity = reg.create();

	reg.emplace<barn::sprite>(entity, make_sprite(renderer, def.sprite));

	reg.emplace<barn::body>(entity, make_body(world_id, def.body));

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::action>(entity, def.action);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::FOE);

	return entity;
}

entt::entity barn::create_projectile(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::projectile_def& def) {
	const entt::entity entity = reg.create();

	reg.emplace<barn::sprite>(entity, make_sprite(renderer, def.sprite));

	reg.emplace<barn::body>(entity, make_body(world_id, def.body));

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::type>(entity, barn::type::PROJECTILE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	return entity;
}
