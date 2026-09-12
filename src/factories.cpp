#include "factories.hpp"
#include "components.hpp"
#include "assets.hpp"

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
		def.frames,
		def.width,
		def.height,
		def.duration,
		barn::get_texture(renderer, def.texture),
		std::chrono::steady_clock::time_point{}
	);
}

static void add_sprite(entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, const barn::sprite_def& def) {
	registry.emplace<barn::component::sprite>(entity,
		def.src_rect,
		def.width,
		def.height,
		barn::get_texture(renderer, def.texture)
	);
}

static void add_track(entt::entity entity, entt::registry& registry, MIX_Mixer* mixer, const barn::track_def& def) {
	barn::audio audio = barn::get_audio(def.audio);
	MIX_Track* track = MIX_CreateTrack(mixer);
	MIX_SetTrackAudio(track, audio.get());
	MIX_PlayTrack(track, def.properties_id);

	registry.emplace<barn::component::track>(entity,
		audio,
		std::unique_ptr<MIX_Track, decltype(&MIX_DestroyTrack)>(track, MIX_DestroyTrack)
	);
}

static void add_properties(entt::entity entity, entt::registry& registry, const barn::base_properties& base) {
	registry.emplace<barn::component::properties>(entity,
		base.health,
		base.attack,
		base.defense,
		base.collide_damage,
		base.speed
	);
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

	if (def.track) {
		add_track(entity, registry, context.mixer, *def.track);
	}

	if (def.properties) {
		add_properties(entity, registry, *def.properties);
	}

	if (def.skillset) {
		registry.emplace<component::skillset>(entity, *def.skillset);

		for (auto& skill : registry.get<component::skillset>(entity)) {
			std::visit([&context](auto&& skill) {
				skill.initialize(context);
			}, skill);
		}
	}

	if (def.keyboard) {
		registry.emplace<component::keyboard>(entity);
	}

	if (def.gamepad) {
		registry.emplace<component::gamepad>(entity, *def.gamepad);
	}

	if (def.transform) {
		registry.emplace<component::transform>(entity, *def.transform);
		if (registry.all_of<component::body>(entity)) {
			const auto& body = registry.get<component::body>(entity);
			b2Body_SetTransform(body.id, def.transform->p, def.transform->q);
		}
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
