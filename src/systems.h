#pragma once

#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <box2d/types.h>

namespace barn {
	void keyboard_system(entt::registry& registry, b2WorldId world_id);

	void joystick_system(entt::registry& registry, b2WorldId world_id);

	void action_system(entt::registry& registry, b2WorldId world_id);

	void sprite_system(entt::registry& registry, sf::RenderWindow& window, sprite& background);
}
