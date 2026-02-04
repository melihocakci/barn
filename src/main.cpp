#include "scenes.h"
#include "constants.h"

#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow window{
		sf::VideoMode::getDesktopMode(),
		"Barn",
		sf::State::Fullscreen
	};

	window.setFramerateLimit(60);

	return barn::main_menu(window);
}
