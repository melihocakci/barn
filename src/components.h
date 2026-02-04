#pragma once

#include <SFML/Window.hpp>

namespace mg {
	struct keyboard_input {
		sf::Keyboard::Scan up;
		sf::Keyboard::Scan down;
		sf::Keyboard::Scan left;
		sf::Keyboard::Scan right;
		sf::Keyboard::Scan fire;
	};

	struct joystick_input {
		unsigned int joystick_id;
		sf::Joystick::Axis horizontal_axis;
		sf::Joystick::Axis vertical_axis;
		unsigned int fire_button;
	};

	struct projectile {
		float speed{};
		sf::Vector2f direction;
	};
}
