#include "factories.hpp"
#include "components.hpp"
#include "assets.hpp"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>

#include <ranges>

static barn::component::body make_body(const barn::body_def& def, b2WorldId world, entt::entity entity) {
	barn::component::body body = b2CreateBody(world, &def.def);

	for (const auto& [shape_def, circle] : def.circles) {
		b2CreateCircleShape(body.id, &shape_def, &circle);
	}

	for (const auto& [shape_def, polygon] : def.polygons) {
		b2CreatePolygonShape(body.id, &shape_def, &polygon);
	}

	b2Body_SetUserData(body.id, new entt::entity{ entity });

	return body;
}

static barn::component::animation make_animation(const barn::animation_def& def, SDL_Renderer* renderer) {
	return barn::component::animation{
		def.frames,
		def.width,
		def.height,
		def.duration,
		barn::get_texture(renderer, def.texture),
		{}
	};
}

static barn::component::animation_list make_animation_list(const barn::animation_list_def& def, SDL_Renderer* renderer) {
	barn::component::animation_list list{};

	if (def.idle_animation) {
		list.idle = make_animation(*def.idle_animation, renderer);
	}
	if (def.attack_animation) {
		list.attack = make_animation(*def.attack_animation, renderer);
	}

	return list;
}

static barn::component::sprite make_sprite(const barn::sprite_def& def, SDL_Renderer* renderer) {
	return barn::component::sprite{
		def.src_rect,
		def.width,
		def.height,
		barn::get_texture(renderer, def.texture)
	};
}

static barn::component::track make_track(const barn::track_def& def, MIX_Mixer* mixer) {
	barn::audio audio = barn::get_audio(def.audio);
	MIX_Track* track = MIX_CreateTrack(mixer);
	MIX_SetTrackAudio(track, audio.get());
	MIX_PlayTrack(track, def.properties_id);

	return barn::component::track{
		audio,
		std::unique_ptr<MIX_Track, decltype(&MIX_DestroyTrack)>(track, MIX_DestroyTrack)
	};
}

static void add_properties(entt::entity entity, entt::registry& registry, const barn::component::base_properties& base) {
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
		registry.emplace<component::body>(entity, make_body(*def.body, context.world_id, entity));
	}

	if (def.animation) {
		registry.emplace<component::animation>(entity, make_animation(*def.animation, context.renderer));
	}

	if (def.animation_list) {
		registry.emplace<component::animation_list>(entity, make_animation_list(*def.animation_list, context.renderer));
	}

	if (def.sprite) {
		registry.emplace<component::sprite>(entity, make_sprite(*def.sprite, context.renderer));
	}

	if (def.track) {
		registry.emplace<component::track>(entity, make_track(*def.track, context.mixer));
	}

	if (def.base_properties) {
		add_properties(entity, registry, *def.base_properties);
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
		registry.emplace<component::previous_transform>(entity, *def.transform);
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
