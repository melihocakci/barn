#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "characters.h"
#include "enemies.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>

static void add_player(entt::registry& reg, sf::Vector2f position, const mg::character& character) {
	const entt::entity entity = reg.create();

	mg::sprite& sprite = reg.emplace<mg::sprite>(entity, character.sprite);
	sprite.scale({ 0.05, 0.05 });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(position);

	mg::hitbox& hitbox = reg.emplace<mg::hitbox>(entity, 10);
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
		scancode::Z,
		scancode::X,
		scancode::C,
		scancode::V);

	reg.emplace<mg::joystick_input>(entity, 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u, 1u, 2u, 3u);
}

static void add_enemy(entt::registry& reg, sf::Vector2f position, const mg::enemy& enemy) {
	const entt::entity entity = reg.create();

	mg::sprite& sprite = reg.emplace<mg::sprite>(entity, enemy.sprite);
	//sprite.scale({ 0.05, 0.05 });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(position);

	mg::hitbox& hitbox = reg.emplace<mg::hitbox>(entity, 10);
	hitbox.setFillColor(sf::Color::Red);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(position);

	reg.emplace<mg::stats>(entity, enemy.stats);
	reg.emplace<mg::action>(entity, enemy.action);
}

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int mg::main_menu(sf::RenderWindow& window) {
	return mg::scene1(window);
}

int mg::scene1(sf::RenderWindow& window) {
	const sf::Texture bg_texture{ "assets/texture/bliss.jpg" };
	mg::sprite background{ bg_texture };

	background.setOrigin({
		background.getTextureRect().size.x / 2.f,
		background.getTextureRect().size.y / 2.f });
	background.setPosition({
		window.getSize().x / 2.f,
		window.getSize().y / 2.f });
	background.setScale({
		static_cast<float>(window.getSize().x) / background.getTextureRect().size.x,
		static_cast<float>(window.getSize().y) / background.getTextureRect().size.y });

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;
	add_player(registry, { window.getSize().x / 2.f, window.getSize().y / 2.f }, playable_characters[0]);
	add_enemy(registry, { window.getSize().x / 2.f, window.getSize().y / 2.f }, set_enemies[0]);

	while (window.isOpen())
	{
		handle_events(window);

		handle_keyboard_inputs(window, registry);

		handle_joystick_inputs(window, registry);

		handle_projectiles(window, registry);

		handle_actions(window, registry);

		window.clear();
		window.draw(background);

		for (auto [entity, sprite] : registry.view<const mg::sprite>().each()) {
			window.draw(sprite);
		}

		for (auto [entity, skills, hitbox] : registry.view<const mg::skillset, const mg::hitbox>().each()) {
			window.draw(hitbox);
		}

		window.display();
	}

	return 0;
}
