#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "static_elements.h"
#include "constants.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>

static void add_player(entt::registry& reg, const project_stable::character& character) {
	const entt::entity entity = reg.create();

	project_stable::sprite& sprite = reg.emplace<project_stable::sprite>(entity, character.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition({ VIRTUAL_WIDTH * 0.5f, VIRTUAL_HEIGHT * 0.8f });

	project_stable::hitbox& hitbox = reg.emplace<project_stable::hitbox>(entity, character.hitbox);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(sprite.getPosition());

	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(2.f);


	reg.emplace<project_stable::properties>(entity, character.stats);

	reg.emplace<project_stable::skillset>(entity, character.skillset);

	reg.emplace<project_stable::type>(entity, project_stable::type::CREATURE);

	reg.emplace<project_stable::alignment>(entity, project_stable::alignment::ALLY);

	using scancode = sf::Keyboard::Scancode;
	reg.emplace<project_stable::player_input>(entity,
		project_stable::keyboard_input{
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

	//reg.emplace<project_stable::player_input>(entity, project_stable::joystick_input{ 0u, sf::Joystick::Axis::X, sf::Joystick::Axis::Y, 0u, 1u, 2u, 3u });
}

static void add_enemy(entt::registry& reg, const project_stable::enemy& enemy) {
	const entt::entity entity = reg.create();

	project_stable::sprite& sprite = reg.emplace<project_stable::sprite>(entity, enemy.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition({ VIRTUAL_WIDTH * 0.5f, VIRTUAL_HEIGHT * 0.3f });

	project_stable::hitbox& hitbox = reg.emplace<project_stable::hitbox>(entity, enemy.hitbox);
	hitbox.setOrigin(hitbox.getGeometricCenter());
	hitbox.setPosition(sprite.getPosition());

	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(2.f);

	reg.emplace<project_stable::properties>(entity, enemy.stats);

	reg.emplace<project_stable::action>(entity, enemy.action);

	reg.emplace<project_stable::type>(entity, project_stable::type::CREATURE);

	reg.emplace<project_stable::alignment>(entity, project_stable::alignment::FOE);
}

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int project_stable::main_menu(sf::RenderWindow& window) {
	return combat_scene(window);
}

int project_stable::combat_scene(sf::RenderWindow& window) {
	sprite background{ texture::bliss };

	background.setScale({ VIRTUAL_WIDTH / background.getTextureRect().size.x, VIRTUAL_HEIGHT / background.getTextureRect().size.y });

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;

	constexpr float border_thickness = 50.f;

	entt::entity top_border = registry.create();
	registry.emplace<hitbox>(top_border, sf::Vector2f{ VIRTUAL_WIDTH + 2.f * border_thickness, border_thickness }).setPosition({ -border_thickness, -border_thickness });
	registry.emplace<type>(top_border, type::OBSTACLE);
	registry.emplace<alignment>(top_border, alignment::NEUTRAL);
	registry.emplace<properties>(top_border);

	entt::entity bottom_border = registry.create();
	registry.emplace<hitbox>(bottom_border, sf::Vector2f{ VIRTUAL_WIDTH + 2.f * border_thickness, border_thickness }).setPosition({ -border_thickness, VIRTUAL_HEIGHT });
	registry.emplace<type>(bottom_border, type::OBSTACLE);
	registry.emplace<alignment>(bottom_border, alignment::NEUTRAL);
	registry.emplace<properties>(bottom_border);

	entt::entity left_border = registry.create();
	registry.emplace<hitbox>(left_border, sf::Vector2f{ border_thickness, VIRTUAL_HEIGHT + 2.f * border_thickness }).setPosition({ -border_thickness, -border_thickness });
	registry.emplace<type>(left_border, type::OBSTACLE);
	registry.emplace<alignment>(left_border, alignment::NEUTRAL);
	registry.emplace<properties>(left_border);

	entt::entity right_border = registry.create();
	registry.emplace<hitbox>(right_border, sf::Vector2f{ border_thickness, VIRTUAL_HEIGHT + 2.f * border_thickness }).setPosition({ VIRTUAL_WIDTH, -border_thickness });
	registry.emplace<type>(right_border, type::OBSTACLE);
	registry.emplace<alignment>(right_border, alignment::NEUTRAL);
	registry.emplace<properties>(right_border);

	add_player(registry, character_templates[0]);
	add_enemy(registry, enemy_templates[0]);

	while (window.isOpen())
	{
		handle_events(window);

		player_input_system(registry);

		trajectory_system(registry);

		action_system(registry);

		hitbox_system(registry);

		window.clear();
		window.draw(background);

		for (auto [entity, sprite] : registry.view<const project_stable::sprite>().each()) {
			window.draw(sprite);
		}

		for (auto [entity, hitbox] : registry.view<const project_stable::hitbox>().each()) {
			window.draw(hitbox);
		}

		window.display();
	}

	return 0;
}
