#pragma once

#include "context.h"

#include <entt/entt.hpp>
#include <box2d/types.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	void property_system(entt::registry& registry);

	void keyboard_system(entt::registry& registry, barn::context& context);

	void gamepad_system(entt::registry& registry, barn::context& context);

	void input_system(entt::registry& registry, barn::context& context);

	void AI_system(entt::registry& registry, barn::context& context);

	void sprite_system(entt::registry& registry, barn::context& context, float alpha, float scale, float offset_x, float offset_y);

	void animation_system(entt::registry& registry, barn::context& context, float alpha, float scale, float offset_x, float offset_y);

	void body_system(entt::registry& registry, barn::context& context);
}
