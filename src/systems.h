#pragma once

#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace barn {
	void keyboard_system(entt::registry& registry);
	
	void joystick_system(entt::registry& registry);

	void trajectory_system(entt::registry& registry);

	void action_system(entt::registry& registry);

	void hitbox_system(entt::registry& registry);

	void sprite_system(entt::registry& registry, sf::RenderWindow& window, sprite& background);
}
