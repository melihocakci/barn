#pragma once

#include "definitions.h"

#include <box2d/types.h>
#include <entt/entt.hpp>

namespace barn {
	void player_factory(entt::registry& reg, const b2WorldId world_id, const barn::character& def);

	void enemy_factory(entt::registry& reg, const b2WorldId world_id, const barn::enemy& def);

	void projectile_factory(entt::registry& reg, const b2WorldId world_id, const barn::projectile& def);

	void border_factory(entt::registry& reg, const b2WorldId world_id);
}
