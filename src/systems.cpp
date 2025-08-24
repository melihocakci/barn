#include "systems.h"
#include "components.h"
#include "constants.h"

#include <entt/entt.hpp>

#include <variant>


void project_stable::player_input_system(entt::registry& registry) {
	for (auto [entity, player_input, hitbox, skillset, properties] : registry.view<const player_input, hitbox, skillset, const properties>().each())
	{
		std::visit([&](const auto& input)
			{
				sf::Vector2f delta{};

				using T = std::decay_t<decltype(input)>;
				if constexpr (std::is_same_v<T, project_stable::keyboard_input>) {
					if (sf::Keyboard::isKeyPressed(input.skill_1))
						skillset.skill_1(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_2))
						skillset.skill_2(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_3))
						skillset.skill_3(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_4))
						skillset.skill_4(registry, entity);

					if (sf::Keyboard::isKeyPressed(input.up))
						delta += sf::Vector2f{ 0, -1 };
					if (sf::Keyboard::isKeyPressed(input.down))
						delta += sf::Vector2f{ 0, 1 };
					if (sf::Keyboard::isKeyPressed(input.left))
						delta += sf::Vector2f{ -1, 0 };
					if (sf::Keyboard::isKeyPressed(input.right))
						delta += sf::Vector2f{ 1, 0 };
				}
				else if constexpr (std::is_same_v<T, project_stable::joystick_input>) {
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_1))
						skillset.skill_1(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_2))
						skillset.skill_2(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_3))
						skillset.skill_3(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_4))
						skillset.skill_4(registry, entity);

					delta = {
						sf::Joystick::getAxisPosition(input.joystick_id, input.horizontal_axis) / 100,
						sf::Joystick::getAxisPosition(input.joystick_id, input.vertical_axis) / 100
					};
				}

				if (delta.length() > 1.f) delta = delta.normalized();
				else if (delta.length() < 0.05f) return;

				delta *= properties.speed;

				hitbox.move(delta);
			}, player_input
		);
	}
}

void project_stable::trajectory_system(entt::registry& registry) {
	for (auto [entity, trajectory, hitbox] : registry.view<const trajectory, hitbox>().each()) {
		sf::Vector2f delta = trajectory.direction.normalized() * trajectory.speed;
		hitbox.move(delta);
	}
}

void project_stable::action_system(entt::registry& registry) {
	for (auto [entity, action] : registry.view<const action>().each()) {
		action(registry, entity);
	}
}

void project_stable::hitbox_system(entt::registry& registry) {
	const auto view = registry.view<hitbox, properties, type, alignment>().each();
	std::vector<entt::entity> entities_to_remove;

	for (auto first_iteretor = view.begin(); first_iteretor != view.end(); ++first_iteretor) {
		auto [first_entity, first_hitbox, first_properties, first_type, first_alignment] = *first_iteretor;
		auto second_iteretor = first_iteretor;

		for (++second_iteretor; second_iteretor != view.end(); ++second_iteretor) {
			auto [second_entity, second_hitbox, second_properties, second_type, second_alignment] = *second_iteretor;

			if (first_type == second_type) {
				continue;
			}
			if (first_alignment == second_alignment) {
				continue;
			}
			else if (!first_hitbox.getGlobalBounds().findIntersection(second_hitbox.getGlobalBounds())) {
				continue;
			}
			else if (first_type == type::OBSTACLE && second_type == type::CREATURE || first_type == type::CREATURE && second_type == type::OBSTACLE) {
				hitbox& obstacle_hitbox = (first_type == type::OBSTACLE) ? first_hitbox : second_hitbox;
				hitbox& creature_hitbox = (first_type == type::CREATURE) ? first_hitbox : second_hitbox;

				float dx = obstacle_hitbox.getGlobalBounds().getCenter().x - creature_hitbox.getGlobalBounds().getCenter().x;
				float dy = obstacle_hitbox.getGlobalBounds().getCenter().y - creature_hitbox.getGlobalBounds().getCenter().y;

				float overlapX = (obstacle_hitbox.getSize().x / 2.f + creature_hitbox.getSize().x / 2.f) - std::abs(dx);
				float overlapY = (obstacle_hitbox.getSize().y / 2.f + creature_hitbox.getSize().y / 2.f) - std::abs(dy);

				if (overlapX < overlapY) {
					// Move along X axis
					if (dx > 0)
						creature_hitbox.move({ -overlapX, 0.f }); // Move left
					else
						creature_hitbox.move({ overlapX, 0.f }); // Move right
				}
				else {
					// Move along Y axis
					if (dy > 0)
						creature_hitbox.move({ 0.f, -overlapY }); // Move up
					else
						creature_hitbox.move({ 0.f, overlapY }); // Move down
				}
			}
			else if (first_type == type::OBSTACLE && second_type == type::PROJECTILE || first_type == type::PROJECTILE && second_type == type::OBSTACLE) {
				entities_to_remove.push_back((first_type == type::PROJECTILE) ? first_entity : second_entity);
			}
			else {
				first_properties.health -= second_properties.attack;
				second_properties.health -= first_properties.attack;
				if (first_properties.health <= 0) {
					entities_to_remove.push_back(first_entity);
				}
				if (second_properties.health <= 0) {
					entities_to_remove.push_back(second_entity);
				}
			}
		}
	}

	for (const auto& entity : entities_to_remove) {
		registry.destroy(entity);
	}
}

void project_stable::sprite_system(entt::registry& registry, sf::RenderWindow& window, sprite& background) {
	const float window_ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	const float target_ratio = VIRTUAL_WIDTH / VIRTUAL_HEIGHT;

	float scale_x = 1.f;
	float scale_y = 1.f;
	float offset_x = 0.f;
	float offset_y = 0.f;

	if (window_ratio > target_ratio) {
		// Window is wider than target
		scale_x = target_ratio / window_ratio;
		offset_x = (1.f - scale_x) / 2.f;
	}
	else {
		// Window is taller than target
		scale_y = window_ratio / target_ratio;
		offset_y = (1.f - scale_y) / 2.f;
	}

	sf::View view{ sf::FloatRect{ { 0.f, 0.f }, { VIRTUAL_WIDTH, VIRTUAL_HEIGHT } } };
	view.setViewport(sf::FloatRect{ { offset_x, offset_y }, { scale_x, scale_y } });


	for (auto [entity, sprite, hitbox] : registry.view<sprite, const hitbox>().each()) {
		sprite.setPosition(hitbox.getPosition());
	}

	window.clear();
	window.setView(view);

	background.setScale({ VIRTUAL_WIDTH / background.getTextureRect().size.x, VIRTUAL_HEIGHT / background.getTextureRect().size.y });
	window.draw(background);

	for (auto [entity, sprite] : registry.view<const project_stable::sprite>().each()) {
		window.draw(sprite);
	}

	for (auto [entity, hitbox] : registry.view<const project_stable::hitbox>().each()) {
		window.draw(hitbox);
	}

	window.setView(window.getDefaultView());

	window.display();
}
