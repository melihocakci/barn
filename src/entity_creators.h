#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace mg {
	void add_miku(entt::registry& reg, sf::Vector2f position);

	void add_bullet(entt::registry& reg, sf::Vector2f position);
}
