#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <steam/steam_api.h>

#include <map>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr float BASE_SPEED = SCREEN_HEIGHT / 120;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), "Teto Pear");
    window.setFramerateLimit(60);

    const sf::Texture texture("res/teto_pear.jpg");
    sf::Sprite sprite(texture);

    const sf::Font font("res/Consolas.ttf");
    sf::Text text(font, "I'm Tetobating so hard", SCREEN_HEIGHT / 12);
    text.setFillColor(sf::Color::Black);

    sf::Music music("res/Kasane Teto - Teto territory.mp3");
    music.play();

    sf::RectangleShape player{ {SCREEN_WIDTH / 10, SCREEN_HEIGHT / 10} };
    player.setFillColor(sf::Color::Black);

    while (window.isOpen())
    {
        static sf::Vector2<int> player_direction = { 0, 0 };

        while (const std::optional event = window.pollEvent())
        {
            std::map<sf::Keyboard::Scancode, sf::Vector2<int>> direction_vectors = {
                    { sf::Keyboard::Scancode::Up, {0, -1} },
                    { sf::Keyboard::Scancode::Down, {0, 1} },
                    { sf::Keyboard::Scancode::Right, {1, 0} },
                    { sf::Keyboard::Scancode::Left, {-1, 0} },
            };

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* pressed_key = event->getIf<sf::Event::KeyPressed>())
            {
                auto search = direction_vectors.find(pressed_key->scancode);
                if (search != direction_vectors.end()) {
                    player_direction += search->second;
                    if (player_direction.x < 0) player_direction.x = -1;
                    if (player_direction.x > 0) player_direction.x = 1;
                    if (player_direction.y < 0) player_direction.y = -1;
                    if (player_direction.y > 0) player_direction.y = 1;
                }
            }
            else if (const auto* pressed_key = event->getIf<sf::Event::KeyReleased>())
            {
                auto search = direction_vectors.find(pressed_key->scancode);
                if (search != direction_vectors.end()) {
                    player_direction -= search->second;
                    if (player_direction.x < 0) player_direction.x = -1;
                    if (player_direction.x > 0) player_direction.x = 1;
                    if (player_direction.y < 0) player_direction.y = -1;
                    if (player_direction.y > 0) player_direction.y = 1;
                }
            }
        }

        player.setPosition(player.getPosition() + sf::Vector2f{ player_direction.x * BASE_SPEED, player_direction.y * BASE_SPEED });

        window.clear();

        window.draw(sprite);
        window.draw(text);
        window.draw(player);

        window.display();
    }
}
