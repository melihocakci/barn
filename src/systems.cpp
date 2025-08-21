#include "systems.h"
#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <variant>

static sf::Rect<float> get_bounds(const sf::RenderWindow& window, float padding) {
	return { {padding, padding}, { window.getSize().x - 2 * padding, window.getSize().y - 2 * padding } };
}

static void move_within_bounds(sf::Transformable& object, sf::Vector2f movement, sf::FloatRect bounds) {
	if (object.getPosition().x + movement.x < bounds.position.x)
		movement.x = bounds.position.x - object.getPosition().x;
	else if (object.getPosition().x + movement.x > bounds.position.x + bounds.size.x)
		movement.x = bounds.position.x + bounds.size.x - object.getPosition().x;

	if (object.getPosition().y + movement.y < bounds.position.y)
		movement.y = bounds.position.y - object.getPosition().y;
	else if (object.getPosition().y + movement.y > bounds.position.y + bounds.size.y)
		movement.y = bounds.position.y + bounds.size.y - object.getPosition().y;

	object.move(movement);
}

void project_stable::handle_player_input(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, player_input, sprite, hitbox, skills, stats] : registry.view<const player_input, sprite, hitbox, skillset, const stats>().each())
	{
		std::visit([&](const auto& input)
			{
				sf::Vector2f delta{};

				using T = std::decay_t<decltype(input)>;
				if constexpr (std::is_same_v<T, project_stable::keyboard_input>) {
					if (sf::Keyboard::isKeyPressed(input.skill_1))
						skills.skill_1(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_2))
						skills.skill_2(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_3))
						skills.skill_3(registry, entity);
					if (sf::Keyboard::isKeyPressed(input.skill_4))
						skills.skill_4(registry, entity);

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
						skills.skill_1(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_2))
						skills.skill_2(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_3))
						skills.skill_3(registry, entity);
					if (sf::Joystick::isButtonPressed(input.joystick_id, input.skill_4))
						skills.skill_4(registry, entity);

					delta = {
						sf::Joystick::getAxisPosition(input.joystick_id, input.horizontal_axis) / 100,
						sf::Joystick::getAxisPosition(input.joystick_id, input.vertical_axis) / 100
					};
				}

				if (delta.length() > 1.f) delta = delta.normalized();
				else if (delta.length() < 0.05f) return;

				move_within_bounds(hitbox, delta * stats.speed, get_bounds(window, hitbox.getRadius()));
				sprite.setPosition(hitbox.getPosition());
			}, player_input
		);
	}
}

void project_stable::handle_projectiles(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, trajectory, sprite, hitbox] : registry.view<const trajectory, sprite, hitbox>().each()) {
		const sf::Rect<float> window_rect{ {0, 0}, { static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) } };
		const sf::Rect<float> sprite_rect = sprite.getGlobalBounds();
		if (!window_rect.findIntersection(sprite_rect)) {
			registry.destroy(entity);
			continue;
		}

		sf::Vector2f delta = trajectory.direction.normalized().componentWiseMul({ trajectory.speed, trajectory.speed });
		sprite.move(delta);
		hitbox.move(delta);
	}
}

void project_stable::handle_actions(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, action] : registry.view<const action>().each()) {
		action(registry, entity);
	}
}

void project_stable::handle_collisions(const sf::RenderWindow& window, entt::registry& registry) {
	const auto view = registry.view<hitbox, stats, alignment>().each();
	std::vector<entt::entity> entities_to_remove;

	for (auto first_iteretor = view.begin(); first_iteretor != view.end(); ++first_iteretor) {
		auto [first_entity, first_hitbox, first_stats, first_affiliation] = *first_iteretor;
		auto second_iteretor = first_iteretor;

		for (++second_iteretor; second_iteretor != view.end(); ++second_iteretor) {
			auto [second_entity, second_hitbox, second_stats, second_affiliation] = *second_iteretor;

			if (first_affiliation == second_affiliation) continue;

			if (first_hitbox.getRadius() + second_hitbox.getRadius() > (first_hitbox.getPosition() - second_hitbox.getPosition()).length()) {
				first_stats.health -= second_stats.attack;
				second_stats.health -= first_stats.attack;
				if (first_stats.health <= 0) {
					entities_to_remove.push_back(first_entity);
				}
				if (second_stats.health <= 0) {
					entities_to_remove.push_back(second_entity);
				}
			}
		}
	}

	for (const auto& entity : entities_to_remove) {
		registry.destroy(entity);
	}
}
