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

static void add_idle_animation(entt::entity entity, FACTORY_PARAMETERS, const barn::animation_def& def) {
	registry.emplace<barn::component::idle_animation>(entity,
		def,
		barn::get_texture(renderer, def.texture),
		std::chrono::steady_clock::time_point{}
	);
}

static void add_properties(entt::entity entity, FACTORY_PARAMETERS, const barn::base_properties& base) {
	registry.emplace<barn::component::properties>(entity,
		base,
		base.health,
		base.attack,
		base.defense,
		base.collide_damage,
		base.speed
	);
}

static void add_skillset(entt::entity entity, FACTORY_PARAMETERS, const barn::skillset_def& def) {
	constexpr std::size_t N = std::tuple_size_v<barn::component::skillset>;

	barn::component::skillset skillset = [&] <std::size_t... I>(std::index_sequence<I...>) {
		return barn::component::skillset{ { barn::skill{ def[I], std::chrono::steady_clock::time_point{} }... } };
	}(std::make_index_sequence<N>{});

	barn::component::skillset& skills = registry.emplace<barn::component::skillset>(entity, skillset);
	for (auto& skill : skills) {
		skill.def.action(ACTION_VARIABLES, barn::action_state::ACTION_INITIALIZE);
	}
}

entt::entity barn::create_entity(FACTORY_PARAMETERS, const barn::entity_def& def) {
	const entt::entity entity = registry.create();

	if (def.body) {
		add_body(entity, FACTORY_VARIABLES, *def.body);
	}

	if (def.idle_animation) {
		add_idle_animation(entity, FACTORY_VARIABLES, *def.idle_animation);
	}

	if (def.properties) {
		add_properties(entity, FACTORY_VARIABLES, *def.properties);
	}

	if (def.skillset) {
		add_skillset(entity, FACTORY_VARIABLES, *def.skillset);
	}

	if (def.action) {
		component::action& action = registry.emplace<component::action>(entity, *def.action);
		action(ACTION_VARIABLES, ACTION_INITIALIZE);
	}

	if (def.player) {
		registry.emplace<component::player>(entity, *def.player);
	}

	if (def.enemy) {
		registry.emplace<component::enemy>(entity);
	}

	if (def.bullet) {
		registry.emplace<component::bullet>(entity);
	}

	if (def.obstacle) {
		registry.emplace<component::obstacle>(entity);
	}

	if (def.background) {
		registry.emplace<component::background>(entity);
	}

	return entity;
}
