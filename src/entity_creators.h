#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "static_elements.h"

namespace mg {
	void add_player(entt::registry& reg, sf::Vector2f position, const character& character);

	void add_bullet(entt::registry& reg, sf::Vector2f position);
}
