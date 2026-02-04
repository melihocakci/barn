#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "characters.h"
#include "enemies.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>

static void add_player(entt::registry& reg, sf::Vector2f position, const project_stable::character& character) {
	const entt::entity entity = reg.create();

	project_stable::sprite& sprite = reg.emplace<project_stable::sprite>(entity, character.sprite);
	sprite.scale({ 0.05, 0.05 });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(position);

	project_stable::hitbox& hitbox = reg.emplace<project_stable::hitbox>(entity, character.hitbox);
	hitbox.setFillColor(sf::Color::Red);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(position);

	reg.emplace<project_stable::stats>(entity, character.stats);

	reg.emplace<project_stable::skillset>(entity, character.skillset);

	reg.emplace<project_stable::alignment>(entity, project_stable::alignment::PLAYER);

	using scancode = sf::Keyboard::Scancode;
	reg.emplace<project_stable::player_input>(entity,
		project_stable::keyboard_input{
			scancode::Up,
			scancode::Down,
			scancode::Left,
			scancode::Right,
			scancode::Z,
			scancode::X,
			scancode::C,
			scancode::V
		}
	);

	//reg.emplace<project_stable::user_input>(entity, project_stable::joystick_input{ 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u, 1u, 2u, 3u });
}

static void add_enemy(entt::registry& reg, sf::Vector2f position, const project_stable::enemy& enemy) {
	const entt::entity entity = reg.create();

	project_stable::sprite& sprite = reg.emplace<project_stable::sprite>(entity, enemy.sprite);
	//sprite.scale({ 0.05, 0.05 });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(position);

	project_stable::hitbox& hitbox = reg.emplace<project_stable::hitbox>(entity, enemy.hitbox);
	hitbox.setFillColor(sf::Color::Red);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(position);

	reg.emplace<project_stable::stats>(entity, enemy.stats);

	reg.emplace<project_stable::action>(entity, enemy.action);

	reg.emplace<project_stable::alignment>(entity, project_stable::alignment::ENEMY);
}

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int project_stable::main_menu(sf::RenderWindow& window) {
	return combat_scene(window);
}

int project_stable::combat_scene(sf::RenderWindow& window) {
	const sf::Texture bg_texture{ "assets/texture/bliss.jpg" };
	project_stable::sprite background{ bg_texture };

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
	add_player(registry, { window.getSize().x / 2.f, window.getSize().y / 2.f + 400 }, character_templates[0]);
	add_enemy(registry, { window.getSize().x / 2.f, window.getSize().y / 2.f }, enemy_templates[0]);

	while (window.isOpen())
	{
		handle_events(window);

		handle_player_input(window, registry);

		handle_projectiles(window, registry);

		handle_actions(window, registry);

		handle_collisions(window, registry);

		window.clear();
		window.draw(background);

		for (auto [entity, sprite] : registry.view<const project_stable::sprite>().each()) {
			window.draw(sprite);
		}

		for (auto [entity, hitbox] : registry.view<const project_stable::hitbox>().each()) {
			window.draw(hitbox);
		}

		window.display();
	}

	return 0;
}
