#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "systems.h"
#include "components.h"
#include "entity_creators.h"

void mg::handle_keyboard_inputs(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, keyboard, sprite, hitbox] : registry.view<const mg::keyboard_input, sf::Sprite, sf::CircleShape>().each()) {
		sf::Vector2f delta{ 0, 0 };
		if (sf::Keyboard::isKeyPressed(keyboard.up) && sprite.getPosition().y > 0)
			delta += sf::Vector2f{ 0, -1 };
		if (sf::Keyboard::isKeyPressed(keyboard.down) && sprite.getPosition().y < window.getSize().y)
			delta += sf::Vector2f{ 0, 1 };
		if (sf::Keyboard::isKeyPressed(keyboard.left) && sprite.getPosition().x > 0)
			delta += sf::Vector2f{ -1, 0 };
		if (sf::Keyboard::isKeyPressed(keyboard.right) && sprite.getPosition().x < window.getSize().x)
			delta += sf::Vector2f{ 1, 0 };

		if (delta.length() > 0) {
			const sf::Vector2f BASE_SPEED = { window.getSize().y / 120.f, window.getSize().y / 120.f };
			delta = delta.normalized().componentWiseMul(BASE_SPEED);
			sprite.move(delta);
			hitbox.move(delta);
		}

		if (sf::Keyboard::isKeyPressed(keyboard.fire))
			add_bullet(registry, hitbox.getPosition());
	}
}

void mg::handle_joystick_inputs(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, joystick, sprite, hitbox] : registry.view<const mg::joystick_input, sf::Sprite, sf::CircleShape>().each()) {
		sf::Vector2f delta{
			sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.horizontal_axis),
			sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.vertical_axis)
		};

		if (delta.length() > 5) {
			const sf::Vector2f BASE_SPEED = { window.getSize().y / 120.f, window.getSize().y / 120.f };
			delta = delta.normalized().componentWiseMul(BASE_SPEED);
			sprite.move(delta);
			hitbox.move(delta);
		}

		if (sf::Joystick::isButtonPressed(joystick.joystick_id, joystick.fire_button))
			add_bullet(registry, hitbox.getPosition());
	}
}

void mg::handle_projectiles(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, projectile, sprite, hitbox] : registry.view<const mg::projectile, sf::Sprite, sf::CircleShape>().each()) {
		const sf::Rect<float> window_rect{ {0, 0}, { static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) } };
		const sf::Rect<float> sprite_rect = sprite.getGlobalBounds();
		if (!window_rect.findIntersection(sprite_rect)) {
			registry.destroy(entity);
			continue;
		}

		sf::Vector2f delta = projectile.direction.normalized().componentWiseMul({ projectile.speed, projectile.speed });
		sprite.move(delta);
		hitbox.move(delta);
	}
}
