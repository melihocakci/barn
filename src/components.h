#pragma once

#include <SFML/Window.hpp>

namespace mg {
	struct keyboard_input {
		sf::Keyboard::Scan up_button;
		sf::Keyboard::Scan down_button;
		sf::Keyboard::Scan left_button;
		sf::Keyboard::Scan right_button;
		sf::Keyboard::Scan fire_button;
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
