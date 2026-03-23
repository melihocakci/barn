#pragma once

#include "presets.h"

#include <box2d/types.h>
#include <entt/entt.hpp>

#define FACTORY_PARAMETERS entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world
#define FACTORY_VARIABLES registry, renderer, mixer, world

namespace barn {
	entt::entity create_borders(FACTORY_PARAMETERS);

	entt::entity create_player(FACTORY_PARAMETERS, const barn::character_preset& def);

	entt::entity create_enemy(FACTORY_PARAMETERS, const barn::enemy_preset& def);

	entt::entity create_bullet(FACTORY_PARAMETERS, const barn::bullet_preset& def);
}
