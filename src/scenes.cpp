#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>

#include "scenes.h"
#include "components.h"
#include "entity_creators.h"
#include "systems.h"

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
	const sf::Texture bg_texture{ "res/teto_pear.jpg" };
	sf::Sprite background{ bg_texture };

	background.setOrigin({
		background.getTextureRect().size.x / 2.f,
		background.getTextureRect().size.y / 2.f });
	background.setPosition({
		window.getSize().x / 2.f,
		window.getSize().y / 2.f });
	background.setScale({
		static_cast<float>(window.getSize().x) / background.getTextureRect().size.x,
		static_cast<float>(window.getSize().y) / background.getTextureRect().size.y });

	sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
	music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;
	add_miku(registry, { window.getSize().x / 2.f, window.getSize().y / 2.f });

	while (window.isOpen())
	{
		handle_events(window);

		handle_keyboard_inputs(window, registry);

		handle_joystick_inputs(window, registry);

		handle_projectiles(window, registry);

		window.clear();
		window.draw(background);

		for (auto [entity, sprite] : registry.view<const sf::Sprite>().each()) {
			window.draw(sprite);
		}

		for (auto [entity, keyboard, hitbox] : registry.view<const mg::keyboard_input, const sf::CircleShape>().each()) {
			window.draw(hitbox);
		}

		for (auto [entity, keyboard, hitbox] : registry.view<const mg::joystick_input, const sf::CircleShape>().each()) {
			window.draw(hitbox);
		}

		window.display();
	}

	return 0;
}
