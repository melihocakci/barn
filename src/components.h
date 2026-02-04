#pragma once

#include <SFML/Window.hpp>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <chrono>

namespace mg {
	using hitbox = sf::CircleShape;

	using sprite = sf::Sprite;

	using action = std::function<void(entt::registry&, entt::entity)>;

	struct keyboard_input {
		sf::Keyboard::Scan up;
		sf::Keyboard::Scan down;
		sf::Keyboard::Scan left;
		sf::Keyboard::Scan right;
		sf::Keyboard::Scan skill_1;
		sf::Keyboard::Scan skill_2;
		sf::Keyboard::Scan skill_3;
		sf::Keyboard::Scan skill_4;
	};

	struct joystick_input {
		unsigned int joystick_id;
		sf::Joystick::Axis horizontal_axis;
		sf::Joystick::Axis vertical_axis;
		unsigned int skill_1;
		unsigned int skill_2;
		unsigned int skill_3;
		unsigned int skill_4;
	};

	using player_input = std::variant<keyboard_input, joystick_input>;

	struct trajectory {
		float speed{};
		sf::Vector2f direction;
	};

	struct skill {
		std::chrono::steady_clock::time_point last_used_time{};
		const std::chrono::milliseconds cooldown_time{};
		const mg::action action;

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
		skill skill_2;
		skill skill_3;
		skill skill_4;
	};

	struct stats {
		float health;
		float speed;
	};
}
