#include "systems.h"
#include "components.h"
#include "constants.h"
#include "utility.h"

#include <entt/entt.hpp>

#include <box2d/box2d.h>

#include <variant>


void barn::keyboard_system(entt::registry& registry, b2WorldId world_id) {
	for (auto [entity, keyboard, body, skillset, properties] : registry.view<const keyboard_controls, body, skillset, const properties>().each())
	{
		if (sf::Keyboard::isKeyPressed(keyboard.skill_1))
			skillset.skill_1(registry, world_id, entity);
		if (sf::Keyboard::isKeyPressed(keyboard.skill_2))
			skillset.skill_2(registry, world_id, entity);
		if (sf::Keyboard::isKeyPressed(keyboard.skill_3))
			skillset.skill_3(registry, world_id, entity);
		if (sf::Keyboard::isKeyPressed(keyboard.skill_4))
			skillset.skill_4(registry, world_id, entity);

		b2Vec2 vec{};
		if (sf::Keyboard::isKeyPressed(keyboard.up))
			vec += { 0, 1 };
		if (sf::Keyboard::isKeyPressed(keyboard.down))
			vec += { 0, -1 };
		if (sf::Keyboard::isKeyPressed(keyboard.left))
			vec += { -1, 0 };
		if (sf::Keyboard::isKeyPressed(keyboard.right))
			vec += { 1, 0 };

		if (length(vec) > 1.f) vec = normalize(vec);

		b2Body_SetLinearVelocity(body, vec * properties.speed);
	}
}

void barn::joystick_system(entt::registry& registry, b2WorldId world_id) {
	for (auto [entity, joystick, body, skillset, properties] : registry.view<const joystick_controls, body, skillset, const properties>().each())
	{
		if (sf::Joystick::isButtonPressed(joystick.joystick_id, joystick.skill_1))
			skillset.skill_1(registry, world_id, entity);
		if (sf::Joystick::isButtonPressed(joystick.joystick_id, joystick.skill_2))
			skillset.skill_2(registry, world_id, entity);
		if (sf::Joystick::isButtonPressed(joystick.joystick_id, joystick.skill_3))
			skillset.skill_3(registry, world_id, entity);
		if (sf::Joystick::isButtonPressed(joystick.joystick_id, joystick.skill_4))
			skillset.skill_4(registry, world_id, entity);

		b2Vec2 vec = {
			sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.horizontal_axis) / 100,
			-sf::Joystick::getAxisPosition(joystick.joystick_id, joystick.vertical_axis) / 100
		};

		if (length(vec) < 0.05f) vec = { 0,0 };

		b2Body_SetLinearVelocity(body, vec * properties.speed);
	}
}

void barn::action_system(entt::registry& registry, b2WorldId world_id) {
	for (auto [entity, action] : registry.view<const action>().each()) {
		action(registry, world_id, entity);
	}
}

void barn::sprite_system(entt::registry& registry, sf::RenderWindow& window, sprite& background) {
	const float window_ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	const float target_ratio = VIRTUAL_WIDTH_PIXELS / VIRTUAL_HEIGHT_PIXELS;

	float scale_x = 1.f;
	float scale_y = 1.f;
	float offset_x = 0.f;
	float offset_y = 0.f;

	if (window_ratio > target_ratio) {
		// Window is wider than target
		scale_x = target_ratio / window_ratio;
		offset_x = (1.f - scale_x) / 2.f;
	}
	else {
		// Window is taller than target
		scale_y = window_ratio / target_ratio;
		offset_y = (1.f - scale_y) / 2.f;
	}

	sf::View view{ sf::FloatRect{ { 0.f, 0.f }, { VIRTUAL_WIDTH_PIXELS, VIRTUAL_HEIGHT_PIXELS } } };
	view.setViewport(sf::FloatRect{ { offset_x, offset_y }, { scale_x, scale_y } });

	for (auto [entity, sprite, body] : registry.view<sprite, const body>().each()) {
		sprite.setPosition(to_pixels(b2Body_GetPosition(body)));
	}

	window.clear();
	window.setView(view);

	background.setScale({ VIRTUAL_WIDTH_PIXELS / background.getTextureRect().size.x, VIRTUAL_HEIGHT_PIXELS / background.getTextureRect().size.y });
	window.draw(background);

	for (auto [entity, sprite] : registry.view<const barn::sprite>().each()) {
		window.draw(sprite);
	}

	window.setView(window.getDefaultView());

	window.display();
}
