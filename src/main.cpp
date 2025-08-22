#include "scenes.h"

#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow window{
		sf::VideoMode::getDesktopMode(),
		"Teto Pear",
		sf::State::Fullscreen
	};

	window.setFramerateLimit(60);

	return project_stable::main_menu(window);
}
