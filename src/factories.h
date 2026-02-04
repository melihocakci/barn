#pragma once

#include "definitions.h"

#include <box2d/types.h>
#include <entt/entt.hpp>

#include <variant>

namespace barn {
	using control_method = std::variant<barn::keyboard_controls, barn::joystick_controls>;

	entt::entity create_player(entt::registry& reg, const b2WorldId world_id, const barn::player_def& def, const barn::control_method& controls);

	entt::entity create_enemy(entt::registry& reg, const b2WorldId world_id, const barn::enemy_def& def);

	entt::entity create_projectile(entt::registry& reg, const b2WorldId world_id, const barn::projectile_def& def);

	entt::entity create_borders(entt::registry& reg, const b2WorldId world_id);
}
