#include "systems.hpp"
#include "components.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "factories.hpp"
#include "assets.hpp"
#include "render.hpp"

#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

void barn::property_system(entt::registry& registry) {
	for (auto [entity, properties] : registry.view<component::properties>().each()) {
		// affects will be applied here

		if (properties.health <= 0) {
			registry.destroy(entity);
		}
	}
}

void barn::keyboard_system(entt::registry& registry, barn::context& context) {
	for (auto [entity, player] : registry.view<component::player, component::keyboard>().each()) {
		const barn::keyboard_controls& controls = context.settings.keyboard_bindings[static_cast<int>(player)];

		const bool* state = SDL_GetKeyboardState(nullptr);

		float axis_x = 0.f, axis_y = 0.f;
		if (state[controls.up])
			axis_y += 1.f;
		if (state[controls.down])
			axis_y += -1.f;
		if (state[controls.left])
			axis_x += -1.f;
		if (state[controls.right])
			axis_x += 1.f;

		component::input& input = registry.get_or_emplace<component::input>(entity);
		input.axis_x = std::fabs(input.axis_x) > std::fabs(axis_x) ? input.axis_x : axis_x;
		input.axis_y = std::fabs(input.axis_y) > std::fabs(axis_y) ? input.axis_y : axis_y;
		for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
			input.skill_pressed[i] |= state[controls.skills[i]];
		}
	}
}

void barn::gamepad_system(entt::registry& registry, barn::context& context) {
	for (auto [entity, player, gamepad] : registry.view<component::player, component::gamepad>().each()) {
		const barn::gamepad_controls& controls = context.settings.gamepad_bindings[static_cast<int>(player)];

		constexpr auto normalize_axis = [](const Sint16 axis) -> float
			{
				constexpr Sint16 DEAD_ZONE = 8000;
				if (abs(axis) < DEAD_ZONE) return 0.f;
				return static_cast<float>(axis > 0 ? axis - DEAD_ZONE : axis + DEAD_ZONE) / (axis > 0 ? 32767 - DEAD_ZONE : 32768 - DEAD_ZONE);
			};

		barn::gamepad& gp = context.gamepads.at(gamepad.id);

		float axis_x = normalize_axis(SDL_GetGamepadAxis(gp.get(), controls.axis_x));
		float axis_y = -normalize_axis(SDL_GetGamepadAxis(gp.get(), controls.axis_y));

		component::input& input = registry.get_or_emplace<component::input>(entity);
		input.axis_x = std::fabs(input.axis_x) > std::fabs(axis_x) ? input.axis_x : axis_x;
		input.axis_y = std::fabs(input.axis_y) > std::fabs(axis_y) ? input.axis_y : axis_y;
		for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
			input.skill_pressed[i] |= SDL_GetGamepadButton(gp.get(), controls.skills[i]);
		}
	}
}

void barn::input_system(entt::registry& registry, barn::context& context) {
	for (auto [entity, input] : registry.view<component::input>().each()) {
		if (registry.all_of<component::body, component::properties>(entity)) {
			auto [body, properties] = registry.get<component::body, component::properties>(entity);
			b2Vec2 vec{
				std::fabs(input.axis_x) > std::fabs(input.axis_x) ? input.axis_x : input.axis_x,
				std::fabs(input.axis_y) > std::fabs(input.axis_y) ? input.axis_y : input.axis_y
			};
			if (length(vec) > 1.f)
				vec = normalize(vec);
			b2Body_SetLinearVelocity(body.id, vec * properties.speed);
		}

		if (registry.all_of<component::skillset>(entity)) {
			component::skillset& skillset = registry.get<component::skillset>(entity);
			for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
				switch (input.skill_state[i]) {
				case component::input::state::PRESSED:
				case component::input::state::HOLDING:
					input.skill_state[i] = input.skill_pressed[i] ? component::input::state::HOLDING : component::input::state::RELEASED;
					break;
				default:
					input.skill_state[i] = input.skill_pressed[i] ? component::input::state::PRESSED : component::input::state::NONE;
				}

				if (std::visit([](auto&& skill) { return skill.on_cooldown(); }, skillset[i])) {
					continue;
				}
				
				std::visit([&](auto&& skill) {
					if (input.skill_state[i] == component::input::state::PRESSED) {
						skill.pressed(context, registry, entity);
					}
					else if (input.skill_state[i] == component::input::state::HOLDING) {
						skill.holding(context, registry, entity);
					}
					else if (input.skill_state[i] == component::input::state::RELEASED) {
						skill.released(context, registry, entity);
					}
				}, skillset[i]);
			}
		}

		input = {};
	}
}

void barn::AI_system(entt::registry& registry, [[maybe_unused]] barn::context& context) {
	for (auto [entity, AI_code] : registry.view<component::AI_code>().each()) {
		switch (AI_code) {
		case component::AI_code::CHASER:
			auto [enemy_body, enemy_stats] = registry.get<component::body, component::properties>(entity);
			b2Vec2 enemy_position = b2Body_GetPosition(enemy_body.id);

			float shortest_distance = -1.f;
			b2Vec2 closest_target{};

			for (auto [entity, _, player_body] : registry.view<component::player, component::body>().each()) {
				const b2Vec2 player_position = b2Body_GetPosition(player_body.id);

				float distance = length(enemy_position - player_position);

				if (distance < shortest_distance || shortest_distance < 0)
				{
					shortest_distance = distance;
					closest_target = player_position;
				}
			}

			if (shortest_distance < 0) {
				b2Body_SetLinearVelocity(enemy_body.id, { 0, 0 });
				return;
			}

			b2Vec2 vel = normalize(closest_target - enemy_position) * enemy_stats.speed;
			b2Body_SetLinearVelocity(enemy_body.id, vel);
		}
	}
}

static barn::component::transform interpolate(barn::component::transform prev, barn::component::transform curr, float alpha) {
	const b2Vec2 pos = prev.p + (curr.p - prev.p) * alpha;

	float dot = prev.q.c * curr.q.c + prev.q.s * curr.q.s;
	if (dot < 0.0f) {
		// Flip curr.q to take the shorter arc
		curr.q.c = -curr.q.c;
		curr.q.s = -curr.q.s;
	}

	// 2. Linearly interpolate the components
	float c = prev.q.c + alpha * (curr.q.c - prev.q.c);
	float s = prev.q.s + alpha * (curr.q.s - prev.q.s);

	// 3. Normalize to ensure it remains a valid prev rotation
	float mag = std::sqrt(c * c + s * s);

	// Handle the exact opposite edge-case (divide by zero protection)
	const b2Rot rot = mag < FLT_EPSILON ? prev.q : b2Rot{ c / mag, s / mag };

	// linear interpolate
	return {
		pos,
		rot
	};
}

void barn::sprite_system(entt::registry& registry, barn::context& context, float alpha, float scale, int offset_x, int offset_y) {
	for (auto [entity, sprite] : registry.view<component::sprite, component::background>().each()) {
		barn::fill_screen(context.renderer, sprite.texture.get());
	}

	for (auto [entity, sprite, transform] : registry.view<component::sprite, component::transform>().each()) {
		barn::draw_texture(
			context.renderer,
			sprite.texture,
			registry.all_of<component::previous_transform>(entity)
			? interpolate(registry.get<component::previous_transform>(entity), transform, alpha) : transform,
			sprite.src_rect ? &*sprite.src_rect : nullptr,
			sprite.width,
			sprite.height,
			scale,
			offset_x,
			offset_y
		);
	}
}

void barn::animation_system(entt::registry& registry, barn::context& context, float alpha, float scale, int offset_x, int offset_y) {
	for (auto [entity, animation, transform] : registry.view<component::animation, component::transform>().each()) {
		barn::draw_animation(
			context.renderer,
			animation,
			interpolate(registry.get_or_emplace<component::previous_transform>(entity), transform, alpha),
			scale,
			offset_x,
			offset_y
		);
	}
}

void barn::animation_list_system(entt::registry& registry, barn::context& context, float alpha, float scale, int offset_x, int offset_y) {
	for (auto [entity, animation_list, transform] : registry.view<component::animation_list, component::transform>().each()) {
		component::animation* animation_ptr = nullptr;

		if (animation_list.current == component::animation_list::type::IDLE && animation_list.idle) {
			animation_ptr = &*animation_list.idle;
		}
		else if (animation_list.current == component::animation_list::type::ATTACK && animation_list.attack) {
			animation_ptr = &*animation_list.attack;
		}

		if (animation_ptr) {
			barn::draw_animation(
				context.renderer,
				*animation_ptr,
				interpolate(registry.get_or_emplace<component::previous_transform>(entity), transform, alpha),
				scale,
				offset_x,
				offset_y
			);
		}
	}
}

void barn::track_system(entt::registry& registry, [[maybe_unused]] barn::context& context) {
	for (auto [entity, track] : registry.view<component::track>().each()) {
		if (!MIX_TrackPlaying(track.track.get())) {
			registry.remove<component::track>(entity);
		}
	}
}

void barn::body_system(entt::registry& registry, barn::context& context) {
	b2World_Step(context.world_id, PHYSICS_TIMESTEP, BOX2D_SUB_STEP_COUNT);

	for (auto [entity, body] : registry.view<component::body>().each()) {
		if (registry.all_of<component::transform>(entity)) {
			registry.emplace_or_replace<component::previous_transform>(entity, registry.get<component::transform>(entity));
			registry.emplace_or_replace<component::transform>(entity, b2Body_GetTransform(body.id));
		}
		else {
			registry.emplace_or_replace<component::transform>(entity, b2Body_GetTransform(body.id));
			registry.emplace_or_replace<component::previous_transform>(entity, registry.get<component::transform>(entity));
		}
	}

	for (auto [entity, body] : registry.view<component::bullet, component::body>().each()) {
		constexpr float BULLET_DESTROY_MARGIN = 2.f;
		const b2Vec2 pos = b2Body_GetPosition(body.id);
		if (pos.x < -BULLET_DESTROY_MARGIN || pos.x > VIRTUAL_WIDTH_METERS + BULLET_DESTROY_MARGIN ||
			pos.y < -BULLET_DESTROY_MARGIN || pos.y > VIRTUAL_HEIGHT_METERS + BULLET_DESTROY_MARGIN) {
			registry.destroy(entity);
		}
	}

	const b2ContactEvents contact_events = b2World_GetContactEvents(context.world_id);

	for (int i = 0; i < contact_events.beginCount; ++i)
	{
		const b2ContactBeginTouchEvent& begin_event = contact_events.beginEvents[i];

		if (!b2Shape_IsValid(begin_event.shapeIdA) || !b2Shape_IsValid(begin_event.shapeIdB)) {
			continue;
		}

		const b2BodyId bodyA = b2Shape_GetBody(begin_event.shapeIdA);
		const entt::entity enttA = *static_cast<entt::entity*>(b2Body_GetUserData(bodyA));
		const b2BodyId bodyB = b2Shape_GetBody(begin_event.shapeIdB);
		const entt::entity enttB = *static_cast<entt::entity*>(b2Body_GetUserData(bodyB));

		if (!registry.all_of<component::properties>(enttA) || !registry.all_of<component::properties>(enttB)) {
			continue;
		}

		component::properties& propA = registry.get<component::properties>(enttA);
		component::properties& propB = registry.get<component::properties>(enttB);
		propA.health -= propB.collide_damage;
		propB.health -= propA.collide_damage;

		if (propA.health <= 0) {
			registry.destroy(enttA);
		}

		if (propB.health <= 0) {
			registry.destroy(enttB);
		}
	}
}
