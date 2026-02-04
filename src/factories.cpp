#include "factories.h"
#include "constants.h"
#include "assets.h"

#include <box2d/box2d.h>
#include <entt/entt.hpp>


void barn::border_factory(entt::registry& reg, const b2WorldId world_id) {
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

	entt::entity obstacle = reg.create();
	reg.emplace<barn::body>(obstacle, body_id);
	reg.emplace<barn::type>(obstacle, barn::type::OBSTACLE);
	reg.emplace<barn::alignment>(obstacle, barn::alignment::NEUTRAL);
	reg.emplace<barn::properties>(obstacle);
}

void barn::player_factory(entt::registry& reg, const b2WorldId world_id, const barn::character& def) {
	const entt::entity entity = reg.create();

	auto& texture = reg.emplace<barn::texture>(entity, barn::load_texture(def.texture));
	auto& sprite = reg.emplace<barn::sprite>(entity, *texture);
	const float scale = def.size.y * PIXELS_PER_METER / sprite.getTextureRect().size.y;
	sprite.setScale({ scale, scale });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2{ VIRTUAL_WIDTH_METERS * 0.5f, VIRTUAL_HEIGHT_METERS * 0.2f };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);
	b2Polygon dynamic_box = b2MakeBox(def.size.x / 2, def.size.y / 2);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 1.0f;
	shape_def.material.friction = 0.3f;
	b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

	reg.emplace<barn::body>(entity, body_id);

	reg.emplace<barn::properties>(entity, def.prop);

	reg.emplace<barn::skillset>(entity, def.skills);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	using scancode = sf::Keyboard::Scancode;
	reg.emplace<barn::keyboard_controls>(entity,
		barn::keyboard_controls{
			scancode::Up,
			scancode::Down,
			scancode::Left,
			scancode::Right,
			scancode::Z,
			scancode::X,
			scancode::C,
			scancode::V
		}
	);

	//reg.emplace<barn::player_input>(entity, barn::joystick_input{ 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u, 1u, 2u, 3u });
}

void barn::enemy_factory(entt::registry& reg, const b2WorldId world_id, const barn::enemy& def) {
	const entt::entity entity = reg.create();

	auto& texture = reg.emplace<barn::texture>(entity, barn::load_texture(def.texture));
	auto& sprite = reg.emplace<barn::sprite>(entity, *texture);
	const float scale = def.size.y * PIXELS_PER_METER / sprite.getTextureRect().size.y;
	sprite.setScale({ scale, scale });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2{ VIRTUAL_WIDTH_METERS * 0.5f, VIRTUAL_HEIGHT_METERS * 0.7f };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);
	b2Polygon dynamic_box = b2MakeBox(def.size.x / 2, def.size.y / 2);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 1.0f;
	shape_def.material.friction = 0.3f;
	b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

	reg.emplace<barn::body>(entity, body_id);

	reg.emplace<barn::properties>(entity, def.prop);

	reg.emplace<barn::action>(entity, def.act);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::FOE);
}

void barn::projectile_factory(entt::registry& reg, const b2WorldId world_id, const barn::projectile& def) {
	const entt::entity entity = reg.create();

	auto& texture = reg.emplace<barn::texture>(entity, barn::load_texture(def.texture));
	auto& sprite = reg.emplace<barn::sprite>(entity, *texture);
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_dynamicBody;
	body_def.position = def.transform.p;
	body_def.linearVelocity = def.velocity;
	b2BodyId body_id = b2CreateBody(world_id, &body_def);
	b2Polygon dynamic_box = b2MakeBox(def.size.x / 2, def.size.y / 2);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 1.0f;
	shape_def.material.friction = 0.3f;
	b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

	reg.emplace<barn::body>(entity, body_id);

	reg.emplace<barn::properties>(entity, def.prop);

	reg.emplace<barn::type>(entity, barn::type::PROJECTILE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);
}
