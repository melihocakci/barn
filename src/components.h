#pragma once

#include <SFML/Window.hpp>
#include <functional>
#include <entt/entt.hpp>
#include <chrono>

namespace mg {
	struct keyboard_input {
		sf::Keyboard::Scan up;
		sf::Keyboard::Scan down;
		sf::Keyboard::Scan left;
		sf::Keyboard::Scan right;
		sf::Keyboard::Scan skill_1;
	};

	struct joystick_input {
		unsigned int joystick_id{};
		sf::Joystick::Axis horizontal_axis;
		sf::Joystick::Axis vertical_axis;
		unsigned int skill_1{};
	};

	struct projectile {
		float speed{};
		sf::Vector2f direction;
	};

	struct stats {
		int health{};
		int max_health{};
		int damage{};
		int armor{};
		int speed{};
	};

	struct skill {
		std::chrono::steady_clock::time_point last_used_time{};
		const std::chrono::milliseconds cooldown_time{};
		const std::function<void(entt::registry&, entt::entity)>& action;

		void operator()(entt::registry& reg, entt::entity ent) {
			using namespace std::chrono;

			auto current_time = steady_clock::now();
			auto time_span = duration_cast<milliseconds>(current_time - last_used_time);

			if (time_span >= cooldown_time) {
				action(reg, ent);
				last_used_time = current_time;
			}
		}
	};

	struct skillset {
		skill skill_1;
	};
}
