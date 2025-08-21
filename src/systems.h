#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace project_stable {
	void handle_player_input(const sf::RenderWindow& window, entt::registry& registry);

	void handle_projectiles(const sf::RenderWindow& window, entt::registry& registry);

	void handle_actions(const sf::RenderWindow& window, entt::registry& registry);
	
	void handle_collisions(const sf::RenderWindow& window, entt::registry& registry);
}
