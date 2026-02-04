#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "definitions.h"
#include "constants.h"
#include "factories.h"
#include "assets.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int barn::main_menu(sf::RenderWindow& window, const b2WorldId world_id) {
	return combat_scene(window, world_id);
}

int barn::combat_scene(sf::RenderWindow& window, const b2WorldId world_id) {
	const auto bliss_texture = load_texture("assets/texture/bliss.jpg");
	sprite background{ *bliss_texture };

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;

	border_factory(registry, world_id);
	player_factory(registry, world_id, character_templates[0]);
	enemy_factory(registry, world_id, enemy_templates[0]);

	sf::Clock clock;

	while (window.isOpen())
	{
		b2World_Step(world_id, clock.restart().asSeconds(), BOX2D_SUB_STEP_COUNT);

		handle_events(window);

		if (window.hasFocus()) {
			keyboard_system(registry, world_id);
			joystick_system(registry, world_id);
		}

		action_system(registry, world_id);

		sprite_system(registry, window, background);
	}

	return 0;
}
