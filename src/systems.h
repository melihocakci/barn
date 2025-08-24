#pragma once

#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace project_stable {
	void player_input_system(entt::registry& registry);

	void trajectory_system(entt::registry& registry);

	void action_system(entt::registry& registry);

	void hitbox_system(entt::registry& registry);

	void sprite_system(entt::registry& registry, sf::RenderWindow& window, sprite& background);
}
