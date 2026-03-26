#include "systems.h"
#include "components.h"
#include "constants.h"
#include "utils.h"
#include "config.h"

#include <box2d/box2d.h>
#include <SDL3/SDL.h>

void barn::property_system(entt::registry& registry) {
	for (auto [entity, properties] : registry.view<component::properties>().each()) {
		// affects will be applied here

		if (properties.health <= 0) {
			registry.destroy(entity);
		}
	}
}

static barn::input get_keyboard_input(barn::component::player player) {
	const barn::config::keyboard_controls& controls = barn::config::keyboard_bindings[static_cast<int>(player)];

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

	return barn::input{
		.axis_x = axis_x,
		.axis_y = axis_y,
		.skill1 = state[controls.skill1],
		.skill2 = state[controls.skill2],
		.skill3 = state[controls.skill3],
		.skill4 = state[controls.skill4],
	};
}

static barn::input get_gamepad_input(barn::component::player player, const barn::component::gamepad& gamepad) {
	const barn::config::gamepad_controls& controls = barn::config::gamepad_bindings[static_cast<int>(player)];

	constexpr auto normalize_axis = [](const Sint16 axis) -> float
		{
			constexpr Sint16 DEAD_ZONE = 8000;
			if (abs(axis) < DEAD_ZONE) return 0.f;
			return static_cast<float>(axis > 0 ? axis - DEAD_ZONE : axis + DEAD_ZONE) / (axis > 0 ? 32767 - DEAD_ZONE : 32768 - DEAD_ZONE);
		};

	return barn::input{
		.axis_x = normalize_axis(SDL_GetGamepadAxis(gamepad.get(), controls.axis_x)),
		.axis_y = -normalize_axis(SDL_GetGamepadAxis(gamepad.get(), controls.axis_y)),
		.skill1 = SDL_GetGamepadButton(gamepad.get(), controls.skill1),
		.skill2 = SDL_GetGamepadButton(gamepad.get(), controls.skill2),
		.skill3 = SDL_GetGamepadButton(gamepad.get(), controls.skill3),
		.skill4 = SDL_GetGamepadButton(gamepad.get(), controls.skill4),
	};
}

static void execute_skill(barn::skill& skill, ACTION_PARAMETERS) {
	using namespace std::chrono;
	const steady_clock::time_point current_time = steady_clock::now();
	const milliseconds time_span = duration_cast<milliseconds>(current_time - skill.last_used_time);
	if (time_span >= skill.def.cooldown) {
		skill.def.action(ACTION_VARIABLES, state);
		skill.last_used_time = current_time;
	}
}

void barn::input_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	const auto view = registry.view<component::player, component::keyboard, component::body, component::skillset, component::properties>();
	for (auto [entity, player, body, skillset, properties] : view.each()) {
		barn::input keyboard_input{}, gamepad_input{};

		if (registry.all_of<barn::component::keyboard>(entity)) {
			keyboard_input = get_keyboard_input(player);
		}

		if (registry.all_of<barn::component::gamepad>(entity)) {
			const barn::component::gamepad& gamepad = registry.get<barn::component::gamepad>(entity);
			gamepad_input = get_gamepad_input(player, gamepad);
		}

		b2Vec2 vec{
			std::fabs(keyboard_input.axis_x) > std::fabs(gamepad_input.axis_x) ? keyboard_input.axis_x : gamepad_input.axis_x,
			std::fabs(keyboard_input.axis_y) > std::fabs(gamepad_input.axis_y) ? keyboard_input.axis_y : gamepad_input.axis_y
		};

		if (length(vec) > 1.f)
			vec = normalize(vec);

		b2Body_SetLinearVelocity(body.id, vec * properties.speed);

		if (keyboard_input.skill1 || gamepad_input.skill1)
			execute_skill(skillset[0], ACTION_VARIABLES, ACTION_RUN);
		if (keyboard_input.skill2 || gamepad_input.skill2)
			execute_skill(skillset[1], ACTION_VARIABLES, ACTION_RUN);
		if (keyboard_input.skill3 || gamepad_input.skill3)
			execute_skill(skillset[2], ACTION_VARIABLES, ACTION_RUN);
		if (keyboard_input.skill4 || gamepad_input.skill4)
			execute_skill(skillset[3], ACTION_VARIABLES, ACTION_RUN);
	}
}

void barn::action_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	for (auto [entity, action] : registry.view<barn::action>().each()) {
		action(ACTION_VARIABLES, ACTION_RUN);
	}
}

// helper lambda to compute interpolated position
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

static void draw_texture(SDL_Renderer* renderer, barn::texture texture, const SDL_FRect* src_rect, std::optional<float> width, std::optional<float> height, barn::component::transform transform)
{
	if (!texture) return;

	const float texture_aspect_ratio = static_cast<float>(texture->w) / texture->h;

	float dest_width, dest_height;
	if (width && height) {
		dest_width = *width;
		dest_height = *height;
	}
	else if (!width && height) {
		dest_width = *height * texture_aspect_ratio;
		dest_height = *height;
	}
	else if (width && !height) {
		dest_width = *width;
		dest_height = *width / texture_aspect_ratio;
	}
	else {
		dest_width = texture->w;
		dest_height = texture->h;
	}

	const SDL_FRect dest_rect = {
		transform.p.x * barn::PIXELS_PER_METER - dest_width / 2,
		barn::VIRTUAL_HEIGHT_PIXELS - transform.p.y * barn::PIXELS_PER_METER - dest_height / 2,
		dest_width,
		dest_height
	};

	SDL_RenderTextureRotated(
		renderer,
		texture.get(),
		src_rect,
		&dest_rect,
		b2Rot_GetAngle(transform.q) * 360 / B2_PI,
		nullptr,
		SDL_FLIP_NONE
	);
}

void barn::draw_system(entt::registry& registry, SDL_Renderer* renderer, float alpha) {
	SDL_RenderClear(renderer);

	for (auto [entity, sprite] : registry.view<component::sprite, component::background>().each()) {
		SDL_FRect dest_rect = { 0, 0, VIRTUAL_WIDTH_PIXELS, VIRTUAL_HEIGHT_PIXELS };

		SDL_RenderTexture(
			renderer,
			sprite.texture.get(),
			nullptr,
			&dest_rect
		);
	}

	for (auto [entity, sprite, body] : registry.view<component::sprite, component::body>().each()) {
		draw_texture(
			renderer,
			sprite.texture,
			sprite.def.src_rect ? &*sprite.def.src_rect : nullptr,
			sprite.def.width,
			sprite.def.height,
			registry.all_of<component::transform>(entity) ?
			interpolate(registry.get<component::transform>(entity), b2Body_GetTransform(body.id), alpha)
			: b2Body_GetTransform(body.id)
		);
	}

	for (auto [entity, idle_animation] : registry.view<component::idle_animation>().each()) {
		if (!registry.all_of<component::animation>(entity)) {
			component::animation& animation = registry.emplace<component::animation>(entity, idle_animation);
			animation.start_time = std::chrono::steady_clock::now();
		}
	}

	for (auto [entity, animation, body] : registry.view<component::animation, component::body>().each()) {
		if (animation.def.frames.empty()) continue;

		using namespace std::chrono;
		const steady_clock::time_point current_time = steady_clock::now();
		long elapsed = duration_cast<milliseconds>(current_time - animation.start_time).count();
		long duration = animation.def.duration.count();

		if (elapsed >= duration) {
			if (registry.all_of<component::idle_animation>(entity)) {
				animation = registry.get<component::idle_animation>(entity);
				animation.start_time = current_time;
				elapsed = 0;
				duration = animation.def.duration.count();
			}
			else {
				registry.remove<component::animation>(entity);
				continue;
			}
		}

		long size = animation.def.frames.size();
		int frame_index = static_cast<double>(elapsed) / duration * size;

		draw_texture(
			renderer,
			animation.texture,
			&animation.def.frames[frame_index],
			animation.def.width,
			animation.def.height,
			registry.all_of<component::transform>(entity) ?
			interpolate(registry.get<component::transform>(entity), b2Body_GetTransform(body.id), alpha)
			: b2Body_GetTransform(body.id)
		);
	}

	SDL_RenderPresent(renderer);
}

void barn::physics_system(entt::registry& registry, b2WorldId world_id) {
	// Update transforms from physics bodies before the step for interpolation
	for (auto [entity, body] : registry.view<component::body>().each()) {
		auto velocity = b2Body_GetLinearVelocity(body.id);
		registry.emplace_or_replace<component::transform>(entity, b2Body_GetTransform(body.id));
	}

	b2World_Step(world_id, PHYSICS_TIMESTEP, BOX2D_SUB_STEP_COUNT);

	for (auto [entity, body] : registry.view<component::bullet, component::body>().each()) {
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

		if (registry.any_of<component::bullet, component::obstacle>(enttA)
			&& registry.any_of<component::bullet, component::obstacle>(enttB))
		{
			entt::entity bullet_entity = registry.any_of<component::bullet>(enttA) ? enttA : enttB;
			registry.destroy(bullet_entity);
		}
	}
}
