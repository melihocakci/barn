#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "systems.h"
#include "components.h"
#include "entity_creators.h"

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

void mg::handle_keyboard_inputs(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, keyboard, sprite, hitbox] : registry.view<const mg::keyboard_input, sf::Sprite, sf::CircleShape>().each()) {
		sf::Vector2f delta{ 0, 0 };
		if (sf::Keyboard::isKeyPressed(keyboard.up_button))
			delta += sf::Vector2f{ 0, -1 };
		if (sf::Keyboard::isKeyPressed(keyboard.down_button))
			delta += sf::Vector2f{ 0, 1 };
		if (sf::Keyboard::isKeyPressed(keyboard.left_button))
			delta += sf::Vector2f{ -1, 0 };
		if (sf::Keyboard::isKeyPressed(keyboard.right_button))
			delta += sf::Vector2f{ 1, 0 };

		const auto bounds = get_bounds(window, hitbox.getRadius());
		move_within_bounds(sprite, delta, bounds);
		move_within_bounds(hitbox, delta, bounds);

		if (sf::Keyboard::isKeyPressed(keyboard.fire_button))
			add_bullet(registry, hitbox.getPosition());
	}
}

void mg::handle_joystick_inputs(const sf::RenderWindow& window, entt::registry& registry) {
	for (auto [entity, joystick, sprite, hitbox] : registry.view<const mg::joystick_input, sf::Sprite, sf::CircleShape>().each()) {
		const sf::Rect<float> sprite_rect = hitbox.getGlobalBounds();

		sf::Vector2f delta{
			sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.horizontal_axis) / 100,
			sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.vertical_axis) / 100
		};

		const auto bounds = get_bounds(window, hitbox.getRadius());
		move_within_bounds(sprite, delta, bounds);
		move_within_bounds(hitbox, delta, bounds);

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
