#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "entity_creators.h"
#include "components.h"
#include "static_elements.h"

void mg::add_player(entt::registry& reg, sf::Vector2f position, const character& character) {
	const entt::entity entity = reg.create();

	sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, character.texture);
	sprite.scale({ 0.05, 0.05 });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(position);

	sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
	hitbox.setFillColor(sf::Color::Red);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(position);

	reg.emplace<mg::skillset>(entity, character.skillset);

	using scancode = sf::Keyboard::Scancode;
	reg.emplace<mg::keyboard_input>(entity,
		scancode::Up,
		scancode::Down,
		scancode::Left,
		scancode::Right,
		scancode::Space);

	reg.emplace<mg::joystick_input>(entity, 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u);
}

void mg::add_bullet(entt::registry& reg, sf::Vector2f position) {
	static const sf::Texture texture{ "res/green-onion.png" };

	const entt::entity entity = reg.create();
	sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, texture);
	sprite.setOrigin({ sprite.getTextureRect().size.x / 2.f , sprite.getTextureRect().size.y / 2.f });
	sprite.setPosition(position);

	sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
	hitbox.setOrigin({ hitbox.getRadius(), hitbox.getRadius() });
	hitbox.setPosition(position);

	reg.emplace<mg::projectile>(entity, 10.f, sf::Vector2f{ 0, -1 });
}
