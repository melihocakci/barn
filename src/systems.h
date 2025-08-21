#pragma once

#include <entt/entt.hpp>

namespace project_stable {
	void player_input_system(entt::registry& registry);

	void trajectory_system(entt::registry& registry);

	void action_system(entt::registry& registry);

	void hitbox_system(entt::registry& registry);
}
