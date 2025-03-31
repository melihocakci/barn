#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

#include <unordered_map>
#include <mutex>

namespace mg {
    struct keyboard_control {
        sf::Keyboard::Scan up;
        sf::Keyboard::Scan down;
        sf::Keyboard::Scan left;
        sf::Keyboard::Scan right;
        sf::Keyboard::Scan fire;
    };

    struct bullet_control {
        float speed;
        sf::Vector2f direction;
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

        bool key_pressed(const sf::Keyboard::Scancode& scancode) const {
            std::lock_guard guard{ keys_lock };
            auto it = keys.find(scancode);
            return it != keys.end() ? it->second : false;
        }

        sf::RenderWindow* operator->() {
            return &window;
        }

        const sf::RenderWindow* operator->() const {
            return &window;
        }

    private:
        sf::RenderWindow window;
        std::unordered_map<sf::Keyboard::Scancode, bool> keys;
        mutable std::mutex keys_lock;
    };

    void add_miku(entt::registry& reg) {
        static const sf::Texture texture{ "res/miku.png" };
        
        const entt::entity entity = reg.create();

        sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, texture);
        sprite.scale({ 0.05, 0.05 });
        sprite.setOrigin({ sprite.getTextureRect().size.x / 2.f , sprite.getTextureRect().size.y / 2.f });

        sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
        hitbox.setFillColor(sf::Color::White);

        using scancode = sf::Keyboard::Scancode;
        reg.emplace<mg::keyboard_control>(entity,
            scancode::Up,
            scancode::Down,
            scancode::Left,
            scancode::Right,
            scancode::Space);
    }

    void add_bullet(entt::registry& reg, const sf::Vector2f position) {
        static const sf::Texture texture{ "res/green-onion.png" };

        const entt::entity entity = reg.create();
        sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, texture);
        sprite.setPosition(position);

        sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
        hitbox.setPosition(position);

        reg.emplace<mg::bullet_control>(entity, 10.f, sf::Vector2f{0, -1});
    }
}

int scene1(mg::window_manager& window) {
    const sf::Texture bg_texture{ "res/teto_pear.jpg" };
    sf::Sprite background{ bg_texture };

    background.setOrigin({ 
        background.getTextureRect().size.x / 2.f, 
        background.getTextureRect().size.y / 2.f });
    background.setPosition({
            window->getSize().x / 2.f,
            window->getSize().y / 2.f });
    background.setScale({
            static_cast<float>(window->getSize().x) / background.getTextureRect().size.x,
            static_cast<float>(window->getSize().y) / background.getTextureRect().size.y });

    sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
    //music.play();

    entt::registry registry;
    mg::add_miku(registry);

    while (window->isOpen())
    {
        window.handle_events();

        registry.view<mg::keyboard_control, sf::Sprite, sf::CircleShape>().each(
            [&window, &registry](const mg::keyboard_control& control, sf::Sprite& sprite, sf::CircleShape& hitbox) {
                sf::Vector2f delta{ 0, 0 };
                if (window.key_pressed(control.up) && sprite.getPosition().y > 0)
                    delta += sf::Vector2f{ 0, -1 };
                if (window.key_pressed(control.down) && sprite.getPosition().y < window->getSize().y)
                    delta += sf::Vector2f{ 0, 1 };
                if (window.key_pressed(control.left) && sprite.getPosition().x > 0)
                    delta += sf::Vector2f{ -1, 0 };
                if (window.key_pressed(control.right) && sprite.getPosition().x < window->getSize().x)
                    delta += sf::Vector2f{ 1, 0 };
                if (window.key_pressed(control.fire))
                    mg::add_bullet(registry, hitbox.getPosition());
                if (delta.length() > 0) {
                    const sf::Vector2f BASE_SPEED = { window->getSize().y / 120.f, window->getSize().y / 120.f };
                    delta = delta.normalized().componentWiseMul(BASE_SPEED);
                    sprite.move(delta);
                    hitbox.move(delta);
                }
            }
        );

        registry.view<mg::bullet_control, sf::Sprite, sf::CircleShape>().each(
            [&window, &registry](const mg::bullet_control& control, sf::Sprite& sprite, sf::CircleShape& hitbox) {
                sf::Vector2f delta = control.direction.normalized().componentWiseMul({ control.speed, control.speed });
                sprite.move(delta);
                hitbox.move(delta);
            }
        );

        window->clear();
        window->draw(background);

        registry.view<sf::Sprite>().each(
            [&window](const sf::Sprite& sprite) { window->draw(sprite); }
        );

        registry.view<mg::keyboard_control, sf::CircleShape>().each(
            [&window](const mg::keyboard_control& control, const sf::CircleShape& hitbox) { window->draw(hitbox); }
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
