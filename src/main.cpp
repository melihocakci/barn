#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <steam/steam_api.h>
#include <entt/entt.hpp>

#include <map>
#include <filesystem>

namespace mg {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr float BASE_SPEED = SCREEN_HEIGHT / 120;

    struct events {
        bool right_pressed;
        bool left_pressed;
        bool up_pressed;
        bool down_pressed;
    };

    events get_events(sf::RenderWindow& window);

    struct sprite {
        sf::Texture txt;
        sf::Sprite spr;

        sprite(std::string_view sprite_file) : txt{ sprite_file }, spr{ txt } {
            spr.scale({ 0.2, 0.2 });
        }
    };

    enum class movement_type {
        player,
    };
}

mg::events mg::get_events(sf::RenderWindow& window) {
    static mg::events events;

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            exit(0);
        }
        else if (const auto* pressed_key = event->getIf<sf::Event::KeyPressed>()) {
            if (pressed_key->scancode == sf::Keyboard::Scancode::Right) events.right_pressed = true;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Left) events.left_pressed = true;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Up) events.up_pressed = true;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Down) events.down_pressed = true;
        }
        else if (const auto* pressed_key = event->getIf<sf::Event::KeyReleased>())
        {
            if (pressed_key->scancode == sf::Keyboard::Scancode::Right) events.right_pressed = false;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Left) events.left_pressed = false;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Up) events.up_pressed = false;
            else if (pressed_key->scancode == sf::Keyboard::Scancode::Down) events.down_pressed = false;
        }
    }

    return events;
}

int scene1(sf::RenderWindow& window) {
    const sf::Texture bg_texture{ "res/teto_pear.jpg" };
    const sf::Sprite background{ bg_texture };

    sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
    music.play();

    entt::registry registry;
    entt::entity player_entity = registry.create();
    registry.emplace<mg::sprite>(player_entity, "res/teto.png");
    registry.emplace<mg::movement_type>(player_entity, mg::movement_type::player);

    while (window.isOpen())
    {
        mg::get_events(window);

        window.clear();
        window.draw(background);

        registry.view<mg::movement_type, mg::sprite>().each(
            [&window](const mg::movement_type& a, mg::sprite& sprite) {
                mg::events events = mg::get_events(window);

                if (events.left_pressed) sprite.spr.move(sf::Vector2f{ -mg::BASE_SPEED, 0 });
                if (events.right_pressed) sprite.spr.move(sf::Vector2f{ mg::BASE_SPEED, 0 });
                if (events.up_pressed) sprite.spr.move(sf::Vector2f{ 0, -mg::BASE_SPEED });
                if (events.down_pressed) sprite.spr.move(sf::Vector2f{ 0, mg::BASE_SPEED });
            }
        );

        registry.view<mg::sprite>().each(
            [&window](const auto& sprite) { window.draw(sprite.spr); }
        );

        window.display();
    }

    return 0;
}

int main() {
    sf::RenderWindow window{ sf::VideoMode{ {mg::SCREEN_WIDTH, mg::SCREEN_HEIGHT } }, "Teto Pear" };
    window.setFramerateLimit(60);

    if (scene1(window)) return 1;
}
