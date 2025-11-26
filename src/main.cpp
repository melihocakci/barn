#include "scenes.h"
#include "constants.h"

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

static sf::RenderWindow create_window() {
	sf::RenderWindow window{
		sf::VideoMode::getDesktopMode(),
		PROJECT_NAME,
		sf::State::Fullscreen
	};

	window.setFramerateLimit(FRAME_RATE);

	return window;
}

static b2WorldId create_world() {
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2{ 0.0f, 0.0f };
	world_def.workerCount = 4;
	b2WorldId world_id = b2CreateWorld(&world_def);
	return world_id;
}

int main() {
	b2WorldId world_id = create_world();

	sf::RenderWindow window = create_window();

	return barn::main_menu(window, world_id);
}
