#pragma once

#include "definitions.h"

#include <box2d/types.h>
#include <entt/entt.hpp>

#include <variant>

namespace barn {
	entt::entity create_borders(entt::registry& reg, b2WorldId world_id);

	entt::entity create_player(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::player_def& def);

	entt::entity create_enemy(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::enemy_def& def);

	entt::entity create_bullet(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world_id, const barn::bullet_def& def);
}
