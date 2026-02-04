#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <box2d/types.h>

#include <functional>
#include <chrono>

namespace barn {
	using body = b2BodyId;

	using sprite = sf::Sprite;

	using action = std::function<void(entt::registry&, b2WorldId, entt::entity)>;

	struct keyboard_controls {
		sf::Keyboard::Scan up;
		sf::Keyboard::Scan down;
		sf::Keyboard::Scan left;
		sf::Keyboard::Scan right;
		sf::Keyboard::Scan skill_1;
		sf::Keyboard::Scan skill_2;
		sf::Keyboard::Scan skill_3;
		sf::Keyboard::Scan skill_4;
	};

	struct joystick_controls {
		unsigned int joystick_id;
		sf::Joystick::Axis horizontal_axis;
		sf::Joystick::Axis vertical_axis;
		unsigned int skill_1;
		unsigned int skill_2;
		unsigned int skill_3;
		unsigned int skill_4;
	};

	struct skill {
		std::chrono::steady_clock::time_point last_used_time{};
		const std::chrono::milliseconds cooldown_time{};
		const barn::action action;

		void operator()(entt::registry& reg, b2WorldId world, entt::entity ent) {
			using namespace std::chrono;

			auto current_time = steady_clock::now();
			auto time_span = duration_cast<milliseconds>(current_time - last_used_time);

			if (time_span >= cooldown_time) {
				action(reg, world, ent);
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

	struct properties {
		int health = 1;
		int attack = 0;
		float speed = 0;
	};

	enum class type {
		CREATURE,
		OBSTACLE,
		PROJECTILE,
		BACKGROUND,
		UI,
	};

	enum class alignment {
		ALLY,
		FOE,
		NEUTRAL,
	};
}
