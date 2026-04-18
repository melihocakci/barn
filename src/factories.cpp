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
	barn::component::skillset& skillset = registry.emplace<barn::component::skillset>(entity);

	for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
		skillset[i] = barn::skill{ def[i], std::chrono::steady_clock::time_point{} };
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

	if (def.ai_code) {
		registry.emplace<component::ai_code>(entity, *def.ai_code);
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
