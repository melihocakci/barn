#include "factories.h"
#include "components.h"
#include "constants.h"
#include "utils.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

static void add_body(entt::entity entity, FACTORY_PARAMETERS, const barn::body_def& def) {
	const barn::component::body& body = registry.emplace<barn::component::body>(entity, b2CreateBody(world, &def.def));

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body.id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body.id, &shape_def, &polygon);
	}

	b2Body_SetUserData(body.id, new entt::entity{ entity });
}

static void add_default_animation(entt::entity entity, FACTORY_PARAMETERS, const barn::animation_def& def) {
	registry.emplace<barn::component::idle_animation>(entity,
		def,
		barn::get_texture(renderer, def.texture),
		std::chrono::steady_clock::time_point{}
	);
}

entt::entity barn::create_borders(FACTORY_PARAMETERS) {
	entt::entity entity = registry.create();

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

	add_body(entity, FACTORY_VARIABLES, body_def);

	return entity;
}

entt::entity barn::create_player(FACTORY_PARAMETERS, const barn::character_preset& def) {
	const entt::entity entity = registry.create();

	add_body(entity, FACTORY_VARIABLES, def.body);

	add_default_animation(entity, FACTORY_VARIABLES, def.idle_animation);

	registry.emplace<component::properties>(entity, def.properties);

	constexpr std::size_t N = std::tuple_size_v<barn::component::skillset>;

	component::skillset skillset = [&] <std::size_t... I>(std::index_sequence<I...>) {
		return barn::component::skillset{ { barn::skill{ def.skillset[I], std::chrono::steady_clock::time_point{} }... } };
	}(std::make_index_sequence<N>{});

	component::skillset& skills = registry.emplace<component::skillset>(entity, skillset);
	for (auto& skill : skills) {
		skill.def.action(ACTION_VARIABLES, ACTION_INITIALIZE);
	}

	return entity;
}

entt::entity barn::create_enemy(FACTORY_PARAMETERS, const barn::enemy_preset& def) {
	const entt::entity entity = registry.create();

	add_body(entity, FACTORY_VARIABLES, def.body);

	add_default_animation(entity, FACTORY_VARIABLES, def.idle_animation);

	registry.emplace<component::properties>(entity, def.properties);

	component::action& action = registry.emplace<component::action>(entity, def.action);
	action(ACTION_VARIABLES, ACTION_INITIALIZE);

	registry.emplace<component::enemy>(entity);

	return entity;
}

entt::entity barn::create_bullet(FACTORY_PARAMETERS, const barn::bullet_preset& def) {
	const entt::entity entity = registry.create();

	add_body(entity, FACTORY_VARIABLES, def.body);

	add_default_animation(entity, FACTORY_VARIABLES, def.idle_animation);

	registry.emplace<component::properties>(entity, def.properties);

	registry.emplace<component::bullet>(entity);

	return entity;
}
