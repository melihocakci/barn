#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

#include <unordered_map>
#include <mutex>

namespace mg {
	struct keyboard_player {
		sf::Keyboard::Scan up;
		sf::Keyboard::Scan down;
		sf::Keyboard::Scan left;
		sf::Keyboard::Scan right;
		sf::Keyboard::Scan fire;
	};

	struct joystick_player {
		unsigned int id;
		unsigned int fire;
	};

	struct projectile {
		float speed{};
		sf::Vector2f direction;
	};

	void add_miku(entt::registry& reg) {
		static const sf::Texture texture{ "res/miku.png" };

		const entt::entity entity = reg.create();

		sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, texture);
		sprite.scale({ 0.05, 0.05 });
		sprite.setOrigin({ sprite.getTextureRect().size.x / 2.f , sprite.getTextureRect().size.y / 2.f });

		sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
		hitbox.setFillColor(sf::Color::White);
		hitbox.setOrigin({ hitbox.getRadius(), hitbox.getRadius() });

		using scancode = sf::Keyboard::Scancode;
		reg.emplace<mg::keyboard_player>(entity,
			scancode::Up,
			scancode::Down,
			scancode::Left,
			scancode::Right,
			scancode::Space);

		reg.emplace<mg::joystick_player>(entity, 0u, 0u);
	}

	void add_bullet(entt::registry& reg, const sf::Vector2f position) {
		static const sf::Texture texture{ "res/green-onion.png" };

		const entt::entity entity = reg.create();
		sf::Sprite& sprite = reg.emplace<sf::Sprite>(entity, texture);
		sprite.setOrigin({ sprite.getTextureRect().size.x / 2.f , sprite.getTextureRect().size.y / 2.f });
		sprite.setPosition(position);

		sf::CircleShape& hitbox = reg.emplace<sf::CircleShape>(entity, 10);
		hitbox.setOrigin({ hitbox.getRadius(), hitbox.getRadius() });
		hitbox.setPosition(position);

		reg.emplace<mg::projectile>(entity, 10.f, sf::Vector2f{ 0, -1 });
	}

	void handle_events(sf::RenderWindow& window) {
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}
	}
}

int scene1(sf::RenderWindow& window) {
	const sf::Texture bg_texture{ "res/teto_pear.jpg" };
	sf::Sprite background{ bg_texture };

	background.setOrigin({
		background.getTextureRect().size.x / 2.f,
		background.getTextureRect().size.y / 2.f });
	background.setPosition({
		window.getSize().x / 2.f,
		window.getSize().y / 2.f });
	background.setScale({
		static_cast<float>(window.getSize().x) / background.getTextureRect().size.x,
		static_cast<float>(window.getSize().y) / background.getTextureRect().size.y });

	sf::Music music{ "res/Kasane Teto - Teto territory.mp3" };
	music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;
	mg::add_miku(registry);

	while (window.isOpen())
	{
		mg::handle_events(window);

		for (auto [entity, keyboard, sprite, hitbox] : registry.view<const mg::keyboard_player, sf::Sprite, sf::CircleShape>().each()) {
			sf::Vector2f delta{ 0, 0 };
			if (sf::Keyboard::isKeyPressed(keyboard.up) && sprite.getPosition().y > 0)
				delta += sf::Vector2f{ 0, -1 };
			if (sf::Keyboard::isKeyPressed(keyboard.down) && sprite.getPosition().y < window.getSize().y)
				delta += sf::Vector2f{ 0, 1 };
			if (sf::Keyboard::isKeyPressed(keyboard.left) && sprite.getPosition().x > 0)
				delta += sf::Vector2f{ -1, 0 };
			if (sf::Keyboard::isKeyPressed(keyboard.right) && sprite.getPosition().x < window.getSize().x)
				delta += sf::Vector2f{ 1, 0 };

			if (delta.length() > 0) {
				const sf::Vector2f BASE_SPEED = { window.getSize().y / 120.f, window.getSize().y / 120.f };
				delta = delta.normalized().componentWiseMul(BASE_SPEED);
				sprite.move(delta);
				hitbox.move(delta);
			}

			if (sf::Keyboard::isKeyPressed(keyboard.fire))
				mg::add_bullet(registry, hitbox.getPosition());
		}

		for (auto [entity, joystick, sprite, hitbox] : registry.view<const mg::joystick_player, sf::Sprite, sf::CircleShape>().each()) {
			sf::Vector2f delta{
				sf::Joystick::getAxisPosition(joystick.id, sf::Joystick::Axis::X),
				sf::Joystick::getAxisPosition(joystick.id, sf::Joystick::Axis::Y)
			};

			if (delta.length() > 5) {
				const sf::Vector2f BASE_SPEED = { window.getSize().y / 120.f, window.getSize().y / 120.f };
				delta = delta.normalized().componentWiseMul(BASE_SPEED);
				sprite.move(delta);
				hitbox.move(delta);
			}

			if (sf::Joystick::isButtonPressed(joystick.id, joystick.fire))
				mg::add_bullet(registry, hitbox.getPosition());
		}

		for (auto [entity, projectile, sprite, hitbox] : registry.view<const mg::projectile, sf::Sprite, sf::CircleShape>().each()) {
			const sf::Rect<float> window_rect{ {0, 0}, { static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) } };
			const sf::Rect<float> sprite_rect = sprite.getGlobalBounds();
			if (!window_rect.findIntersection(sprite_rect)) {
				registry.destroy(entity);
				continue;
			}

			sf::Vector2f delta = projectile.direction.normalized().componentWiseMul({ projectile.speed, projectile.speed });
			sprite.move(delta);
			hitbox.move(delta);
		}

		window.clear();
		window.draw(background);

		for (auto [entity, sprite] : registry.view<const sf::Sprite>().each()) {
			window.draw(sprite);
		}

		for (auto [entity, keyboard, hitbox] : registry.view<const mg::keyboard_player, const sf::CircleShape>().each()) {
			window.draw(hitbox);
		}

		for (auto [entity, keyboard, hitbox] : registry.view<const mg::joystick_player, const sf::CircleShape>().each()) {
			window.draw(hitbox);
		}

		window.display();
	}

	return 0;
}

int main() {
	sf::RenderWindow window{
		sf::VideoMode::getDesktopMode(),
			"Teto Pear",
			sf::State::Fullscreen
	};

	window.setFramerateLimit(60);

	if (scene1(window)) return 1;
}
