#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace mg {
	void handle_keyboard_inputs(const sf::RenderWindow& window, entt::registry& registry);

	void handle_joystick_inputs(const sf::RenderWindow& window, entt::registry& registry);

	void handle_projectiles(const sf::RenderWindow& window, entt::registry& registry);
}
