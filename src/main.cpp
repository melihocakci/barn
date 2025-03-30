#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <steam/steam_api.h>
#include <entt/entt.hpp>

#include <unordered_map>
#include <mutex>

namespace mg {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr float BASE_SPEED = SCREEN_HEIGHT / 120;

    const auto up_key = sf::Keyboard::Scancode::Up;
    const auto down_key = sf::Keyboard::Scancode::Down;
    const auto left_key = sf::Keyboard::Scancode::Left;
    const auto right_key = sf::Keyboard::Scancode::Right;

    enum class control {
        player,
    };

    class window_manager {
    public:
        window_manager(sf::RenderWindow&& w) : window{ std::move(w) } {
            window.setFramerateLimit(60);
        }

        bool key_pressed(const sf::Keyboard::Scancode& scancode) {
            handle_events();
            std::lock_guard guard{ keys_lock };
            auto it = keys.find(scancode);
            return it != keys.end() ? it->second : false;
        }

        sf::RenderWindow* operator->() {
            return &window;
        }
    private:
        void handle_events() {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>()) {
                    exit(0);
                }
                else if (const auto* pressed_key = event->getIf<sf::Event::KeyPressed>()) {
                    std::lock_guard guard{ keys_lock };
                    keys[pressed_key->scancode] = true;
                }
                else if (const auto* released_key = event->getIf<sf::Event::KeyReleased>())
                {
                    std::lock_guard guard{ keys_lock };
                    keys[released_key->scancode] = false;
                }
            }
        }

        sf::RenderWindow window;
        std::unordered_map<sf::Keyboard::Scancode, bool> keys;
        std::mutex keys_lock;
    };
}

int scene1(mg::window_manager& window) {
    const sf::Texture bg_texture{ "res/teto_pear.jpg" };
    const sf::Texture teto_texture{ "res/teto.png" };

    const sf::Sprite background{ bg_texture };

    sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
    //music.play();

    entt::registry registry;

    entt::entity player_entity = registry.create();
    registry.emplace<sf::Sprite>(player_entity, teto_texture);
    registry.emplace<mg::control>(player_entity, mg::control::player);

    while (true)
    {
        registry.view<mg::control, sf::Sprite>().each(
            [&window](const mg::control& control, sf::Sprite& sprite) {
                if (window.key_pressed(mg::up_key)) sprite.move(sf::Vector2f{ 0, -mg::BASE_SPEED });
                if (window.key_pressed(mg::down_key)) sprite.move(sf::Vector2f{ 0, mg::BASE_SPEED });
                if (window.key_pressed(mg::left_key)) sprite.move(sf::Vector2f{ -mg::BASE_SPEED, 0 });
                if (window.key_pressed(mg::right_key)) sprite.move(sf::Vector2f{ mg::BASE_SPEED, 0 });
            }
        );

        window->clear();
        window->draw(background);

        registry.view<sf::Sprite>().each(
            [&window](const sf::Sprite& sprite) { window->draw(sprite); }
        );

        window->display();
    }

    return 0;
}

int main() {
    mg::window_manager window_manager{ sf::RenderWindow{ sf::VideoMode{ {mg::SCREEN_WIDTH, mg::SCREEN_HEIGHT } }, "Teto Pear" } };

    if (scene1(window_manager)) return 1;
}
