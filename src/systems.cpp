#include "systems.h"
#include "components.h"
#include "constants.h"
#include "utils.h"
#include "config.h"

#include <box2d/box2d.h>
#include <SDL3/SDL.h>

static void execute_skill(barn::skill& skill, ACTION_PARAMETERS) {
	using namespace std::chrono;
	const steady_clock::time_point current_time = steady_clock::now();
	const milliseconds time_span = duration_cast<milliseconds>(current_time - skill.last_used_time);
	if (time_span >= skill.cooldown) {
		skill.action(ACTION_VARIABLES);
		skill.last_used_time = current_time;
	}
}

void barn::keyboard_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	const auto view = registry.view<barn::keyboard, barn::player, barn::body, barn::skillset, barn::properties>();
	for (auto [entity, player, body, skillset, properties] : view.each())
	{
		const keyboard_controls& controls = barn::config::keyboard_bindings[player];

		const bool* state = SDL_GetKeyboardState(nullptr);

		if (state[controls.skill1])
			execute_skill(skillset[0], ACTION_VARIABLES);
		if (state[controls.skill2])
			execute_skill(skillset[1], ACTION_VARIABLES);
		if (state[controls.skill3])
			execute_skill(skillset[2], ACTION_VARIABLES);
		if (state[controls.skill4])
			execute_skill(skillset[3], ACTION_VARIABLES);

		b2Vec2 vec{};
		if (state[controls.up])
			vec += { 0, 1 };
		if (state[controls.down])
			vec += { 0, -1 };
		if (state[controls.left])
			vec += { -1, 0 };
		if (state[controls.right])
			vec += { 1, 0 };

		if (length(vec) > 1.f) vec = normalize(vec);

		b2Body_SetLinearVelocity(body.id, vec * properties.speed);
	}
}

void barn::gamepad_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	const auto view = registry.view<barn::gamepad, barn::player, barn::body, barn::skillset, barn::properties>();
	for (auto [entity, gamepad, player, body, skillset, properties] : view.each())
	{
		const gamepad_controls& controls = barn::config::gamepad_bindings[player];

		if (SDL_GetGamepadButton(gamepad.get(), controls.skill1))
			execute_skill(skillset[0], ACTION_VARIABLES);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill2))
			execute_skill(skillset[1], ACTION_VARIABLES);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill3))
			execute_skill(skillset[2], ACTION_VARIABLES);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill4))
			execute_skill(skillset[3], ACTION_VARIABLES);

		constexpr auto normalize_axis = [](const Sint16 axis) -> float
			{
				constexpr Sint16 DEAD_ZONE = 8000;
				if (abs(axis) < DEAD_ZONE) return 0.f;
				return static_cast<float>(axis > 0 ? axis - DEAD_ZONE : axis + DEAD_ZONE) / (axis > 0 ? 32767 - DEAD_ZONE : 32768 - DEAD_ZONE);
			};

		b2Vec2 vec = {
			normalize_axis(SDL_GetGamepadAxis(gamepad.get(), controls.axis_x)),
			-normalize_axis(SDL_GetGamepadAxis(gamepad.get(), controls.axis_y))
		};

		if (length(vec) > 1.f) vec = normalize(vec);

		b2Body_SetLinearVelocity(body.id, vec * properties.speed);
	}
}

void barn::action_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	for (auto [entity, action] : registry.view<barn::action>().each()) {
		action(ACTION_VARIABLES);
	}
}

void barn::draw_system(entt::registry& registry, SDL_Renderer* renderer, float alpha) {
	SDL_RenderClear(renderer);

	for (auto [entity, sprite] : registry.view<barn::sprite, barn::background>().each()) {
		SDL_FRect dest_rect = { 0, 0, VIRTUAL_WIDTH_PIXELS, VIRTUAL_HEIGHT_PIXELS };

		SDL_RenderTexture(
			renderer,
			sprite.texture.get(),
			nullptr,
			&dest_rect
		);
	}

	// helper lambda to compute interpolated position
	constexpr auto interpolate = [](barn::transform prev, barn::transform curr, float alpha) -> barn::transform {
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

		// 3. Normalize to ensure it remains a valid prev.qtion
		float mag = std::sqrt(c * c + s * s);

		// Handle the exact opposite edge-case (divide by zero protection)
		const b2Rot rot = mag < FLT_EPSILON ? prev.q : b2Rot{ c / mag, s / mag };

		// linear interpolate
		return {
			pos,
			rot
		};
	};

	for (auto [entity, sprite, body] : registry.view<barn::sprite, barn::body>().each()) {
		if (!sprite.texture) continue;

		const float texture_aspect_ratio = static_cast<float>(sprite.texture->w) / sprite.texture->h;

		float width{}, height{};
		if (sprite.width && sprite.height) {
			width = *sprite.width;
			height = *sprite.height;
		}
		else if (!sprite.width && sprite.height) {
			height = *sprite.height;
			width = height * texture_aspect_ratio;
		}
		else if (sprite.width && !sprite.height) {
			width = *sprite.width;
			height = width / texture_aspect_ratio;
		}
		else {
			width = sprite.texture->w;
			height = sprite.texture->h;
		}

		const barn::transform transform = 
			registry.all_of<barn::transform>(entity) ?
			interpolate(registry.get<barn::transform>(entity), b2Body_GetTransform(body.id), alpha)
			: b2Body_GetTransform(body.id);

		SDL_FRect dest_rect = {
			transform.p.x * PIXELS_PER_METER - width / 2,
			VIRTUAL_HEIGHT_PIXELS - transform.p.y * PIXELS_PER_METER - height / 2,
			width,
			height
		};

		SDL_RenderTextureRotated(
			renderer,
			sprite.texture.get(),
			sprite.src_rect ? &(*sprite.src_rect) : nullptr,
			&dest_rect,
			b2Rot_GetAngle(transform.q) * 360 / B2_PI,
			nullptr,
			SDL_FLIP_NONE
		);
	}

	for (auto [entity, animation, body] : registry.view<barn::animation, barn::body>().each()) {
		if (!animation.texture) continue;

		const float texture_aspect_ratio = static_cast<float>(animation.texture->w) / animation.texture->h;

		float width{}, height{};
		if (animation.width && animation.height) {
			width = *animation.width;
			height = *animation.height;
		}
		else if (!animation.width && animation.height) {
			height = *animation.height;
			width = height * texture_aspect_ratio;
		}
		else if (animation.width && !animation.height) {
			width = *animation.width;
			height = width / texture_aspect_ratio;
		}
		else {
			width = animation.texture->w;
			height = animation.texture->h;
		}

		const barn::transform transform =
			registry.all_of<barn::transform>(entity) ?
			interpolate(registry.get<barn::transform>(entity), b2Body_GetTransform(body.id), alpha)
			: b2Body_GetTransform(body.id);

		SDL_FRect dest_rect = {
			transform.p.x * PIXELS_PER_METER - width / 2,
			VIRTUAL_HEIGHT_PIXELS - transform.p.y * PIXELS_PER_METER - height / 2,
			width,
			height
		};

		SDL_RenderTextureRotated(
			renderer,
			animation.texture.get(),
			&animation.frames[animation.current_frame_index],
			&dest_rect,
			b2Rot_GetAngle(transform.q) * 360 / B2_PI,
			nullptr,
			SDL_FLIP_NONE
		);

		using namespace std::chrono;
		const steady_clock::time_point current_time = steady_clock::now();
		if (current_time - animation.last_frame_time >= animation.frame_duration) {
			animation.current_frame_index = (animation.current_frame_index + 1) % animation.frames.size();
			animation.last_frame_time = current_time;
			animation.loop_count = (animation.loop_count == -1) ? -1 : animation.loop_count - 1;
			if (animation.loop_count == 0) {
				registry.remove<barn::animation>(entity);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void barn::physics_system(entt::registry& registry, b2WorldId world_id) {
	// Update transforms from physics bodies before the step for interpolation
	for (auto [entity, body] : registry.view<barn::body>().each()) {
		registry.emplace_or_replace<barn::transform>(entity, b2Body_GetTransform(body.id));
	}

	b2World_Step(world_id, PHYSICS_TIMESTEP, BOX2D_SUB_STEP_COUNT);

	for (auto [entity, body] : registry.view<barn::bullet, barn::body>().each()) {
		constexpr float BULLET_DESTROY_MARGIN = 2.f;
		const b2Vec2 pos = b2Body_GetPosition(body.id);
		if (pos.x < -BULLET_DESTROY_MARGIN || pos.x > VIRTUAL_WIDTH_METERS + BULLET_DESTROY_MARGIN ||
			pos.y < -BULLET_DESTROY_MARGIN || pos.y > VIRTUAL_HEIGHT_METERS + BULLET_DESTROY_MARGIN) {
			registry.destroy(entity);
		}
	}

	const b2ContactEvents contact_events = b2World_GetContactEvents(world_id);

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

		if (!registry.all_of<barn::category>(enttA) || !registry.all_of<barn::category>(enttB)) {
			continue;
		}

		const barn::category typeA = registry.get<barn::category>(enttA);
		const barn::category typeB = registry.get<barn::category>(enttB);

		const auto [first, first_type] = typeA < typeB ? std::pair{ enttA, typeA } : std::pair{ enttB, typeB };
		const auto [second, second_type] = typeA < typeB ? std::pair{ enttB, typeB } : std::pair{ enttA, typeA };

		if (first_type & category::ALLY) {
			if (second_type & category::FOE) {
				barn::properties& first_prop = registry.get<barn::properties>(first);
				barn::properties& second_prop = registry.get<barn::properties>(second);
				first_prop.health -= second_prop.attack;
				if (first_prop.health <= 0) {
					registry.destroy(first);
				}
			}
			else if (second_type & category::FOE_BULLET) {
				barn::properties& first_prop = registry.get<barn::properties>(first);
				barn::properties& second_prop = registry.get<barn::properties>(second);
				first_prop.health -= second_prop.attack;
				registry.destroy(second);
				if (first_prop.health <= 0) {
					registry.destroy(first);
				}
			}
		}
		else if (first_type & category::FOE) {
			if (second_type & category::ALLY_BULLET) {
				barn::properties& first_prop = registry.get<barn::properties>(first);
				barn::properties& second_prop = registry.get<barn::properties>(second);
				first_prop.health -= second_prop.attack;
				registry.destroy(second);
				if (first_prop.health <= 0) {
					registry.destroy(first);
				}
			}
		}
	}

	for (int i = 0; i < contact_events.endCount; ++i)
	{
		b2ContactEndTouchEvent& end_event = contact_events.endEvents[i];

		if (!b2Shape_IsValid(end_event.shapeIdA) || !b2Shape_IsValid(end_event.shapeIdB)) {
			continue;
		}

		const b2BodyId bodyA = b2Shape_GetBody(end_event.shapeIdA);
		const entt::entity enttA = *static_cast<entt::entity*>(b2Body_GetUserData(bodyA));
		const b2BodyId bodyB = b2Shape_GetBody(end_event.shapeIdB);
		const entt::entity enttB = *static_cast<entt::entity*>(b2Body_GetUserData(bodyB));

		if (!registry.all_of<barn::category>(enttA) || !registry.all_of<barn::category>(enttB)) {
			continue;
		}

		const auto typeA = registry.get<barn::category>(enttA);
		const auto typeB = registry.get<barn::category>(enttB);

		const auto [first, first_type] = typeA < typeB ? std::pair{ enttA, typeA } : std::pair{ enttB, typeB };
		const auto [second, second_type] = typeA < typeB ? std::pair{ enttB, typeB } : std::pair{ enttA, typeA };

		if (first_type & (category::ALLY_BULLET | category::FOE_BULLET)) {
			if (second_type & category::OBSTACLE) {
				registry.destroy(first);
			}
		}
	}
}
