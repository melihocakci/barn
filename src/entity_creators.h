#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace mg {
	void add_miku(entt::registry& reg);

	void add_bullet(entt::registry& reg, const sf::Vector2f position);
}
