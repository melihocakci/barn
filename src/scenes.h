#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/types.h>

namespace barn {
	int main_menu(sf::RenderWindow& window, const b2WorldId world_id);

	int combat_scene(sf::RenderWindow& window, const b2WorldId world_id);
}
