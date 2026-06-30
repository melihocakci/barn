#include "systems.h"
#include "components.h"
#include "constants.h"
#include "utils.h"
#include "factories.h"
#include "assets.h"

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

		component::input& input = registry.emplace_or_replace<component::input>(entity);
		input.axis_x = std::fabs(input.axis_x) > std::fabs(axis_x) ? input.axis_x : axis_x;
		input.axis_y = std::fabs(input.axis_y) > std::fabs(axis_y) ? input.axis_y : axis_y;
		for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
			input.skills[i] |= state[controls.skills[i]];
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

		SDL_Gamepad* gp = context.gamepads[gamepad.id].get();

		float axis_x = normalize_axis(SDL_GetGamepadAxis(gp, controls.axis_x));
		float axis_y = -normalize_axis(SDL_GetGamepadAxis(gp, controls.axis_y));

		component::input& input = registry.emplace_or_replace<component::input>(entity);
		input.axis_x = std::fabs(input.axis_x) > std::fabs(axis_x) ? input.axis_x : axis_x;
		input.axis_y = std::fabs(input.axis_y) > std::fabs(axis_y) ? input.axis_y : axis_y;
		for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
			input.skills[i] |= SDL_GetGamepadButton(gp, controls.skills[i]);
		}
	}
}

static void execute_skill(barn::skill_code skill_code, entt::entity entity, entt::registry& registry, barn::context& context) {
	using namespace barn;

	static const b2BodyDef default_body_def = [] {
		b2BodyDef def = b2DefaultBodyDef();
		def.type = b2_dynamicBody;
		def.fixedRotation = true;
		return def;
		}();

	static const b2ShapeDef bullet_shape_def = [] {
		b2ShapeDef def = b2DefaultShapeDef();
		def.filter.categoryBits = barn::category::ALLY_BULLET;
		def.filter.maskBits = barn::category::ENEMY | barn::category::OBSTACLE;
		return def;
		}();

	switch (skill_code) {
	case barn::skill_code::GREEN_ONION:
		auto [player_body, player_prop] = registry.get<component::body, component::properties>(entity);

		MIX_PlayAudio(context.mixer, get_audio(audios::weiii).get());

		b2BodyDef body_def = default_body_def;
		body_def.type = b2_kinematicBody;
		body_def.position = b2Body_GetPosition(player_body.id);
		body_def.linearVelocity = { 0.f, 10.f };
		body_def.angularVelocity = B2_PI;

		barn::entity_def def{
			.body = barn::body_def{
				.def = body_def,
				.circles{
					{bullet_shape_def, b2Circle{{}, 0.25f}}
				}
			},
			.idle_animation = animation_def{
				.texture = textures::green_onion,
				.frames = { SDL_FRect{0.f, 0.f, 260.f, 280.f} },
				.width = 1.f * PIXELS_PER_METER,
			},
			.properties = base_properties{
				.collide_damage = player_prop.attack,
			},
			.bullet = component::bullet{}
		};

		barn::create_entity(registry, context, def);
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

		if (registry.any_of<component::skillset>(entity)) {
			component::skillset& skillset = registry.get<component::skillset>(entity);
			for (int i = 0; i < barn::SKILLSET_SIZE; ++i) {
				if (input.skills[i]) {
					using namespace std::chrono;
					const steady_clock::time_point current_time = steady_clock::now();
					const milliseconds time_span = duration_cast<milliseconds>(current_time - skillset[i].last_used_time);
					if (time_span < skillset[i].def.cooldown) {
						continue;
					}
					execute_skill(skillset[i].def.code, entity, registry, context);
					skillset[i].last_used_time = current_time;
				}
			}
		}

		input = {};
	}
}

void barn::AI_system(entt::registry& registry, barn::context& context) {
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

static void draw_texture(SDL_Renderer* renderer, barn::texture texture, barn::component::transform transform, const SDL_FRect* src_rect = nullptr, std::optional<float> width = std::nullopt, std::optional<float> height = std::nullopt)
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

void barn::sprite_system(entt::registry& registry, barn::context& context, float alpha) {
	for (auto [entity, sprite, transform] : registry.view<component::sprite, component::transform>().each()) {
		draw_texture(
			context.renderer,
			sprite.texture,
			registry.all_of<component::previous_transform>(entity)
			? interpolate(registry.get<component::previous_transform>(entity), transform, alpha) : transform,
			sprite.def.src_rect ? &*sprite.def.src_rect : nullptr,
			sprite.def.width,
			sprite.def.height
		);
	}
}

void barn::animation_system(entt::registry& registry, barn::context& context, float alpha) {
	for (auto [entity, idle_animation] : registry.view<component::idle_animation>().each()) {
		if (!registry.all_of<component::animation>(entity)) {
			component::animation& animation = registry.emplace<component::animation>(entity, idle_animation);
			animation.start_time = std::chrono::steady_clock::now();
		}
	}

	for (auto [entity, animation, transform] : registry.view<component::animation, component::transform>().each()) {
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
			context.renderer,
			animation.texture,
			registry.all_of<component::previous_transform>(entity)
			? interpolate(registry.get<component::previous_transform>(entity), transform, alpha) : transform,
			&animation.def.frames[frame_index],
			animation.def.width,
			animation.def.height
		);
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
