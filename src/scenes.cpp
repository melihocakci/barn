#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "static_elements.h"
#include "constants.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

static void add_player(entt::registry& reg, const b2WorldId world_id, const barn::character& character) {
	const entt::entity entity = reg.create();

	barn::sprite& sprite = reg.emplace<barn::sprite>(entity, character.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT_PIXELS * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT_PIXELS * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2{ VIRTUAL_WIDTH_METERS * 0.5f, VIRTUAL_HEIGHT_METERS * 0.2f };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);
	b2Polygon dynamic_box = b2MakeBox(1.0f, 1.0f);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 1.0f;
	shape_def.material.friction = 0.3f;
	b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

	reg.emplace<barn::body>(entity, body_id);

	reg.emplace<barn::properties>(entity, character.properties);

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

static void add_enemy(entt::registry& reg, const b2WorldId world_id, const barn::enemy& enemy) {
	const entt::entity entity = reg.create();

	barn::sprite& sprite = reg.emplace<barn::sprite>(entity, enemy.sprite);
	sprite.setScale({
		VIRTUAL_HEIGHT_PIXELS * 0.25f / sprite.getTextureRect().size.y,
		VIRTUAL_HEIGHT_PIXELS * 0.25f / sprite.getTextureRect().size.y
		});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2{ VIRTUAL_WIDTH_METERS * 0.5f, VIRTUAL_HEIGHT_METERS * 0.7f };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);
	b2Polygon dynamic_box = b2MakeBox(1.0f, 1.0f);
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 1.0f;
	shape_def.material.friction = 0.3f;
	b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

	reg.emplace<barn::body>(entity, body_id);

	reg.emplace<barn::properties>(entity, enemy.stats);

	reg.emplace<barn::action>(entity, enemy.action);

	reg.emplace<barn::type>(entity, barn::type::CREATURE);

	reg.emplace<barn::alignment>(entity, barn::alignment::FOE);
}

static void add_borders(entt::registry& reg, const b2WorldId world_id) {
	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_staticBody;
	body_def.position = { 0, 0 };
	b2BodyId body_id = b2CreateBody(world_id, &body_def);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = 0.f;
	shape_def.material.friction = 0.3f;

	constexpr float half_width = 2.f;

	b2Polygon top_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS + half_width }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &top_rect);

	b2Polygon bottom_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, -half_width }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &bottom_rect);

	b2Polygon left_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { -half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &left_rect);

	b2Polygon right_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { VIRTUAL_WIDTH_METERS + half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2CreatePolygonShape(body_id, &shape_def, &right_rect);

	entt::entity obstacle = reg.create();
	reg.emplace<barn::body>(obstacle, body_id);
	reg.emplace<barn::type>(obstacle, barn::type::OBSTACLE);
	reg.emplace<barn::alignment>(obstacle, barn::alignment::NEUTRAL);
	reg.emplace<barn::properties>(obstacle);
}

static void handle_events(sf::RenderWindow& window) {
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

int barn::main_menu(sf::RenderWindow& window, const b2WorldId world_id) {
	return combat_scene(window, world_id);
}

int barn::combat_scene(sf::RenderWindow& window, const b2WorldId world_id) {
	sprite background{ texture::bliss };

	sf::Music music{ "assets/audio/Kasane Teto - Teto territory.mp3" };
	//music.play();
	music.setLooping(true);
	music.setVolume(20.f);

	entt::registry registry;

	add_borders(registry, world_id);
	add_player(registry, world_id, character_templates[0]);
	add_enemy(registry, world_id, enemy_templates[0]);

	while (window.isOpen())
	{
		handle_events(window);

		if (window.hasFocus()) {
			keyboard_system(registry, world_id);
			joystick_system(registry, world_id);
		}

		action_system(registry, world_id);

		sprite_system(registry, window, background);

		b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
	}

	return 0;
}
