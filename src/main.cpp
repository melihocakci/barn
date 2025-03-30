#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

#include <unordered_map>
#include <mutex>

namespace mg {
    struct keyboard_controls {
        sf::Keyboard::Scan up;
        sf::Keyboard::Scan down;
        sf::Keyboard::Scan left;
        sf::Keyboard::Scan right;
    };

    class window_manager {
    public:
        window_manager(sf::RenderWindow&& w) : window{ std::move(w) } {
            window.setFramerateLimit(60);
        }

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

        bool key_pressed(const sf::Keyboard::Scancode& scancode) {
            std::lock_guard guard{ keys_lock };
            auto it = keys.find(scancode);
            return it != keys.end() ? it->second : false;
        }

        sf::RenderWindow* operator->() {
            return &window;
        }

    private:
        sf::RenderWindow window;
        std::unordered_map<sf::Keyboard::Scancode, bool> keys;
        std::mutex keys_lock;
    };
}

int scene1(mg::window_manager& window) {
    const sf::Texture bg_texture{ "res/teto_pear.jpg" };
    const sf::Texture teto_texture{ "res/miku.png" };

    sf::Sprite background{ bg_texture };
    const auto bg_size = background.getTextureRect().size;
    const auto wd_size = window->getSize();
    background.setOrigin({ bg_size.x / 2.f, bg_size.y / 2.f });
    background.setPosition({ wd_size.x / 2.f, wd_size.y / 2.f });
    background.setScale({ static_cast<float>(wd_size.x) / bg_size.x, static_cast<float>(wd_size.y) / bg_size.y });

    sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
    //music.play();

    entt::registry registry;

    entt::entity player_entity = registry.create();
    registry.emplace<sf::Sprite>(player_entity, teto_texture).scale({ 0.05, 0.05 });
    using scancode = sf::Keyboard::Scancode;
    registry.emplace<mg::keyboard_controls>(player_entity, scancode::Up, scancode::Down, scancode::Left, scancode::Right);

    while (window->isOpen())
    {
        window.handle_events();

        registry.view<mg::keyboard_controls, sf::Sprite>().each(
            [&window](const mg::keyboard_controls& control, sf::Sprite& sprite) {
                const sf::Vector2f BASE_SPEED = { window->getSize().y / 120.f, window->getSize().y / 120.f };
                sf::Vector2f delta{ 0, 0 };
                if (window.key_pressed(control.up)) delta += sf::Vector2f{ 0, -1 };
                if (window.key_pressed(control.down)) delta += sf::Vector2f{ 0, 1 };
                if (window.key_pressed(control.left)) delta += sf::Vector2f{ -1, 0 };
                if (window.key_pressed(control.right)) delta += sf::Vector2f{ 1, 0 };
                if (delta.length() > 0)
                    sprite.move(delta.normalized().componentWiseMul(BASE_SPEED));
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
    mg::window_manager window{
        sf::RenderWindow{
            sf::VideoMode::getDesktopMode(),
            "Teto Pear",
            sf::State::Fullscreen
        }
    };

    if (scene1(window)) return 1;
}
