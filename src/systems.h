#pragma once

#include <entt/entt.hpp>
#include <box2d/types.h>
#include <SDL3/SDL.h>

namespace barn {
	void keyboard_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id);

	void gamepad_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id);

	void action_system(entt::registry& registry, SDL_Renderer* renderer, b2WorldId world_id);

	void sprite_system(entt::registry& registry, SDL_Renderer* renderer);
}
