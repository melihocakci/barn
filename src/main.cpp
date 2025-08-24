#include "scenes.h"
#include "constants.h"

#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow window{
		sf::VideoMode::getDesktopMode(),
		"Project Stable",
		sf::State::Fullscreen
	};

	window.setFramerateLimit(60);

	const float window_ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	const float target_ratio = VIRTUAL_WIDTH / VIRTUAL_HEIGHT;

	float scale_x = 1.f;
	float scale_y = 1.f;
	float offset_x = 0.f;
	float offset_y = 0.f;

	if (window_ratio > target_ratio) {
		// Window is wider than target
		scale_x = target_ratio / window_ratio;
		offset_x = (1.f - scale_x) / 2.f;
	}
	else {
		// Window is taller than target
		scale_y = window_ratio / target_ratio;
		offset_y = (1.f - scale_y) / 2.f;
	}

	sf::View view{ sf::FloatRect{ { 0.f, 0.f }, { VIRTUAL_WIDTH, VIRTUAL_HEIGHT } } };
	view.setViewport(sf::FloatRect{ { offset_x, offset_y }, { scale_x, scale_y } });

	window.setView(view);

	return project_stable::main_menu(window);
}
