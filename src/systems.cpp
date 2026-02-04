#include "systems.h"
#include "components.h"
#include "constants.h"
#include "utils.h"
#include "config.h"

#include <box2d/box2d.h>
#include <SDL3/SDL.h>

void barn::keyboard_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id) {
	const auto view = registry.view<barn::keyboard, barn::player, barn::body, barn::skillset, barn::properties>();
	for (auto [entity, player, body, skillset, properties] : view.each())
	{
		const keyboard_controls controls = barn::keyboard_players[player];

		const bool* state = SDL_GetKeyboardState(nullptr);

		if (state[controls.skill1])
			skillset.skill1(registry, renderer, world_id, entity);
		if (state[controls.skill2])
			skillset.skill2(registry, renderer, world_id, entity);
		if (state[controls.skill3])
			skillset.skill3(registry, renderer, world_id, entity);
		if (state[controls.skill4])
			skillset.skill4(registry, renderer, world_id, entity);

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

void barn::gamepad_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id) {
	const auto view = registry.view<barn::gamepad, barn::player, barn::body, barn::skillset, barn::properties>();
	for (auto [entity, gamepad, player, body, skillset, properties] : view.each())
	{
		const gamepad_controls controls = barn::gamepad_players[player];

		if (SDL_GetGamepadButton(gamepad.get(), controls.skill1))
			skillset.skill1(registry, renderer, world_id, entity);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill2))
			skillset.skill2(registry, renderer, world_id, entity);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill3))
			skillset.skill3(registry, renderer, world_id, entity);
		if (SDL_GetGamepadButton(gamepad.get(), controls.skill4))
			skillset.skill4(registry, renderer, world_id, entity);

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

void barn::action_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id) {
	for (auto [entity, action] : registry.view<barn::action>().each()) {
		action(registry, renderer, world_id, entity);
	}
}

void barn::sprite_system(entt::registry& registry, SDL_Renderer* renderer) {
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

	for (auto [entity, sprite, body] : registry.view<barn::sprite, barn::body>().each()) {
		b2Vec2 pos = b2Body_GetPosition(body.id);

		const float width = sprite.width ? *sprite.width : static_cast<float>(sprite.texture->w);
		const float height = sprite.height ? *sprite.height : static_cast<float>(sprite.texture->h);

		SDL_FRect dest_rect = {
			pos.x * PIXELS_PER_METER - width / 2,
			VIRTUAL_HEIGHT_PIXELS - pos.y * PIXELS_PER_METER - height / 2,
			width,
			height
		};

		SDL_RenderTexture(
			renderer,
			sprite.texture.get(),
			sprite.src_rect ? &(*sprite.src_rect) : nullptr,
			&dest_rect
		);
	}

	SDL_RenderPresent(renderer);
}
