#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "definitions.h"
#include "constants.h"
#include "factories.h"
#include "assets.h"
#include "config.h"

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
	const auto bliss_texture = get_texture("assets/texture/bliss.jpg");
	sprite background{ *bliss_texture };

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;

	create_borders(registry, world_id);

	player_def player = character_templates[0];
	player.body.body.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS / 4 };
	create_player(registry, world_id, player, config::player1);

	enemy_def enemy = enemy_templates[0];
	enemy.body.body.position = { VIRTUAL_WIDTH_METERS / 2, 3 * VIRTUAL_HEIGHT_METERS / 4 };
	create_enemy(registry, world_id, enemy);

	sf::Clock clock;

	while (window.isOpen())
	{
		b2World_Step(world_id, 1.f / 240, BOX2D_SUB_STEP_COUNT);

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
