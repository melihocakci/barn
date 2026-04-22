#pragma once

#include <entt/entt.hpp>
#include <box2d/types.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	void property_system(entt::registry& registry);

	void keyboard_system(entt::registry& registry);

	void gamepad_system(entt::registry& registry);

	void input_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world);

	void AI_system(entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world);

	void sprite_system(entt::registry& registry, SDL_Renderer* renderer, float alpha);

	void animation_system(entt::registry& registry, SDL_Renderer* renderer, float alpha);

	void body_system(entt::registry& registry, b2WorldId world_id);
}
