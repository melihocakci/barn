#include <array>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "components.h"
#include "static_elements.h"

const mg::skill mg::basic_attack = {
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		const auto& player_hitbox = reg.get<sf::CircleShape>(player_entity);

		static const sf::Texture texture{ "res/green-onion.png" };

		const entt::entity bullet_entity = reg.create();
		sf::Sprite& bullet_sprite = reg.emplace<sf::Sprite>(bullet_entity, texture);
		bullet_sprite.setOrigin({ bullet_sprite.getTextureRect().size.x / 2.f , bullet_sprite.getTextureRect().size.y / 2.f });
		bullet_sprite.setPosition(player_hitbox.getPosition());

		sf::CircleShape& bullet_hitbox = reg.emplace<sf::CircleShape>(bullet_entity, 10);
		bullet_hitbox.setOrigin({ bullet_hitbox.getRadius(), bullet_hitbox.getRadius() });
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<mg::projectile>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });
	},
};

const std::array<mg::character, 1> mg::playable_characters = {
	mg::character{
		.name = "Hatsune Miku",
		.texture = sf::Texture{ "res/miku.png" },
		.health = 100,
		.mana = 50,
		.attack = 10,
		.defense = 5,
		.speed = 3,
		.skillset = {
			basic_attack,
		}
	}
};
