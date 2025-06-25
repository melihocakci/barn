#include "systems.h"
#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <variant>

static sf::Rect<float> get_bounds(const sf::RenderWindow& window, float padding) {
	return { {padding, padding}, { window.getSize().x - 2 * padding, window.getSize().y - 2 * padding } };
}

static void move_within_bounds(sf::Transformable& object, sf::Vector2f delta, const sf::Rect<float>& bounds) {
	if (delta.length() < 0.05f) {
		return;
	}
	else if (delta.length() > 1.f) {
		delta = delta.normalized();
	}

	const sf::Vector2f BASE_SPEED = { bounds.size.x / 120.f, bounds.size.x / 120.f };
	delta = delta.componentWiseMul(BASE_SPEED);

	if (object.getPosition().x + delta.x < bounds.position.x) delta.x = bounds.position.x - object.getPosition().x;
	else if (object.getPosition().x + delta.x > bounds.position.x + bounds.size.x) delta.x = bounds.position.x + bounds.size.x - object.getPosition().x;

	if (object.getPosition().y + delta.y < bounds.position.y) delta.y = bounds.position.y - object.getPosition().y;
	else if (object.getPosition().y + delta.y > bounds.position.y + bounds.size.y) delta.y = bounds.position.y + bounds.size.y - object.getPosition().y;

	object.move(delta);
}

void mg::handle_player_input(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, player_input, sprite, hitbox, skills] : registry.view<const mg::player_input, mg::sprite, mg::hitbox, mg::skillset>().each()) {
        std::visit(
			[&](const auto& input) {
				sf::Vector2f delta{};

				using T = std::decay_t<decltype(input)>;
				if constexpr (std::is_same_v<T, mg::keyboard_input>) {
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
				else if constexpr (std::is_same_v<T, mg::joystick_input>) {
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

				const auto bounds = get_bounds(window, hitbox.getRadius());
				move_within_bounds(hitbox, delta, bounds);
				sprite.setPosition(hitbox.getPosition());
			},
            player_input
		);
	}
}

void mg::handle_projectiles(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, trajectory, sprite, hitbox] : registry.view<const mg::trajectory, mg::sprite, mg::hitbox>().each()) {
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

void mg::handle_actions(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, action] : registry.view<const mg::action>().each()) {
		action(registry, entity);
	}
}
