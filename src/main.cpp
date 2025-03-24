#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

int main() {
    sf::RenderWindow window;
    window.create(sf::VideoMode({800, 600}), "SFML window");
    std::string dummy;
    std::getline(std::cin, dummy);
    return 0;
}
