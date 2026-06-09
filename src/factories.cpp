#include "factories.h"
#include "components.h"
#include "constants.h"
#include "assets.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

static void add_body(entt::entity entity, entt::registry& registry, b2WorldId world, const barn::body_def& def) {
	const barn::component::body& body = registry.emplace<barn::component::body>(entity, b2CreateBody(world, &def.def));

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body.id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body.id, &shape_def, &polygon);
	}

	b2Body_SetUserData(body.id, new entt::entity{ entity });
}

static void add_idle_animation(entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, const barn::animation_def& def) {
	registry.emplace<barn::component::idle_animation>(entity,
		def,
		barn::get_texture(renderer, def.texture),
		std::chrono::steady_clock::time_point{}
	);
}

static void add_sprite(entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, const barn::sprite_def& def) {
	registry.emplace<barn::component::sprite>(entity,
		def,
		barn::get_texture(renderer, def.texture)
	);
}

static void add_properties(entt::entity entity, entt::registry& registry, const barn::base_properties& base) {
	registry.emplace<barn::component::properties>(entity,
		base,
		base.health,
		base.attack,
		base.defense,
		base.collide_damage,
		base.speed
	);
}

static void add_skillset(entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, const barn::skillset_def& def) {
	barn::component::skillset& skillset = registry.emplace<barn::component::skillset>(entity);

	for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
		skillset[i].def = def[i];
		for (const auto& texture_def : def[i].assets.textures) {
			skillset[i].assets.textures.push_back(barn::get_texture(renderer, texture_def));
		}
		for (const auto& audio_def : def[i].assets.audios) {
			skillset[i].assets.audios.push_back(barn::get_audio(audio_def));
		}
	}
}

entt::entity barn::create_entity(entt::registry& registry, barn::context& context, const barn::entity_def& def) {
	const entt::entity entity = registry.create();

	if (def.body) {
		add_body(entity, registry, context.world_id, *def.body);
	}

	if (def.idle_animation) {
		add_idle_animation(entity, registry, context.renderer, *def.idle_animation);
	}

	if (def.sprite) {
		add_sprite(entity, registry, context.renderer, *def.sprite);
	}

	if (def.properties) {
		add_properties(entity, registry, *def.properties);
	}

	if (def.skillset) {
		add_skillset(entity, registry, context.renderer, *def.skillset);
	}

	if (def.AI_code) {
		registry.emplace<component::AI_code>(entity, *def.AI_code);
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
