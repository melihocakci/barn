#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "static_elements.h"
#include "constants.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>

static void add_player(entt::registry& reg, const barn::character& character) {
	const entt::entity entity = reg.create();

	barn::sprite& sprite = reg.emplace<barn::sprite>(entity, character.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	barn::hitbox& hitbox = reg.emplace<barn::hitbox>(entity, character.hitbox);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition({ VIRTUAL_WIDTH * 0.5f, VIRTUAL_HEIGHT * 0.8f });

	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(2.f);


	reg.emplace<barn::properties>(entity, character.stats);

	reg.emplace<barn::skillset>(entity, character.skillset);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::ALLY);

	using scancode = sf::Keyboard::Scancode;
	reg.emplace<barn::keyboard_controls>(entity,
		barn::keyboard_controls{
			scancode::Up,
			scancode::Down,
			scancode::Left,
			scancode::Right,
			scancode::Z,
			scancode::X,
			scancode::C,
			scancode::V
		}
	);

	//reg.emplace<barn::player_input>(entity, barn::joystick_input{ 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u, 1u, 2u, 3u });
}

static void add_enemy(entt::registry& reg, const barn::enemy& enemy) {
	const entt::entity entity = reg.create();

	barn::sprite& sprite = reg.emplace<barn::sprite>(entity, enemy.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	barn::hitbox& hitbox = reg.emplace<barn::hitbox>(entity, enemy.hitbox);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition({ VIRTUAL_WIDTH * 0.5f, VIRTUAL_HEIGHT * 0.3f });

	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(2.f);

	reg.emplace<barn::properties>(entity, enemy.stats);

	reg.emplace<barn::action>(entity, enemy.action);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::FOE);
}

static void add_obstacle(entt::registry& reg, const std::pair<float, float> position, const std::pair<float, float> size) {
	entt::entity top_border = reg.create();
	reg.emplace<barn::hitbox>(top_border, sf::Vector2f{ size.first, size.second }).setPosition({ position.first, position.second });
	reg.emplace<barn::type>(top_border, barn::type::OBSTACLE);
	reg.emplace<barn::alignment>(top_border, barn::alignment::NEUTRAL);
	reg.emplace<barn::properties>(top_border);
}

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int barn::main_menu(sf::RenderWindow& window) {
	return combat_scene(window);
}

int barn::combat_scene(sf::RenderWindow& window) {
	sprite background{ texture::bliss };

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;

	constexpr float border_thickness = 50.f;

	add_obstacle(registry, { -border_thickness, -border_thickness }, { VIRTUAL_WIDTH + 2.f * border_thickness, border_thickness });
	add_obstacle(registry, { -border_thickness, VIRTUAL_HEIGHT }, { VIRTUAL_WIDTH + 2.f * border_thickness, border_thickness });
	add_obstacle(registry, { -border_thickness, -border_thickness }, { border_thickness, VIRTUAL_HEIGHT + 2.f * border_thickness });
	add_obstacle(registry, { VIRTUAL_WIDTH, -border_thickness }, { border_thickness, VIRTUAL_HEIGHT + 2.f * border_thickness });

	add_player(registry, character_templates[0]);
	add_enemy(registry, enemy_templates[0]);

	while (window.isOpen())
	{
		handle_events(window);

		keyboard_system(registry);

		joystick_system(registry);

		trajectory_system(registry);

		action_system(registry);

		hitbox_system(registry);

		sprite_system(registry, window, background);
	}

	return 0;
}
