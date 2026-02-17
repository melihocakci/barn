#pragma once

#include <entt/entt.hpp>
#include <box2d/types.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	void keyboard_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world);

	void gamepad_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world);

	void action_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world);

	void draw_system(entt::registry& registry, SDL_Renderer* renderer);

	void physics_system(entt::registry& registry, b2WorldId world_id);
}
