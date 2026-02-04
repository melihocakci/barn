#pragma once

#include "components.h"

namespace barn::config {
	barn::keyboard_controls player1{
		.up = sf::Keyboard::Scancode::Up,
		.down = sf::Keyboard::Scancode::Down,
		.left = sf::Keyboard::Scancode::Left,
		.right = sf::Keyboard::Scancode::Right,
		.skill_1 = sf::Keyboard::Scancode::Q,
		.skill_2 = sf::Keyboard::Scancode::W,
		.skill_3 = sf::Keyboard::Scancode::E,
		.skill_4 = sf::Keyboard::Scancode::R,
	};
}
