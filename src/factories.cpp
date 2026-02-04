#include "factories.h"
#include "constants.h"
#include "assets.h"
#include "components.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

static void add_body(entt::registry& reg, const entt::entity entity, const b2WorldId world_id, const barn::body_def& def) {
	b2BodyId body_id = b2CreateBody(world_id, &def.body);

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body_id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body_id, &shape_def, &polygon);
	}

	reg.emplace<barn::body>(entity, body_id);
}

static void add_sprite(entt::registry& reg, const entt::entity entity, const barn::sprite_def& def) {
	auto& texture = reg.emplace<barn::texture>(entity, barn::get_texture(def.texture));
	auto& sprite = reg.emplace<barn::sprite>(entity, *texture);
	const float scale = def.size.y * PIXELS_PER_METER / sprite.getTextureRect().size.y;
	sprite.setScale({ scale, scale });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
}

entt::entity barn::create_borders(entt::registry& reg, const b2WorldId world_id) {
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

entt::entity barn::create_player(entt::registry& reg, const b2WorldId world_id, const barn::player_def& def, const barn::control_method& controls) {
	const entt::entity entity = reg.create();

	add_sprite(reg, entity, def.sprite);

	add_body(reg, entity, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::skillset>(entity, def.skillset);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	std::visit(
		[&reg, &entity](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			reg.emplace<T>(entity, arg);
		},
		controls
	);

	return entity;
}

entt::entity barn::create_enemy(entt::registry& reg, const b2WorldId world_id, const barn::enemy_def& def) {
	const entt::entity entity = reg.create();

	add_sprite(reg, entity, def.sprite);

	add_body(reg, entity, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::action>(entity, def.action);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::FOE);

	return entity;
}

entt::entity barn::create_projectile(entt::registry& reg, const b2WorldId world_id, const barn::projectile_def& def) {
	const entt::entity entity = reg.create();

	add_sprite(reg, entity, def.sprite);

	add_body(reg, entity, world_id, def.body);

	reg.emplace<barn::properties>(entity, def.properties);

	reg.emplace<barn::type>(entity, barn::type::PROJECTILE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	return entity;
}
