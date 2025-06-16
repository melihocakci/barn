#include "characters.h"
#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <array>

static const sf::Texture miku_texture{ "assets/texture/miku.png" };

const mg::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, entt::entity player_entity) {
	},
};

const mg::skill green_onion_attack = {
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		static const sf::Texture bullet_texture{ "assets/texture/green-onion.png" };

		const auto& player_hitbox = reg.get<sf::CircleShape>(player_entity);

		const entt::entity bullet_entity = reg.create();
		sf::Sprite& bullet_sprite = reg.emplace<sf::Sprite>(bullet_entity, bullet_texture);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());
		bullet_sprite.setPosition(player_hitbox.getPosition());

		sf::CircleShape& bullet_hitbox = reg.emplace<sf::CircleShape>(bullet_entity, 10);
		bullet_hitbox.setOrigin(bullet_hitbox.getGeometricCenter());
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<mg::trajectory>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });
	},
};

const std::array<mg::character, 1> mg::playable_characters = {
	mg::character{
		.name = "Hatsune Miku",
		.sprite = sf::Sprite{ miku_texture },
		.skillset = {
			green_onion_attack,
			none,
			none,
			none,
		}
	}
};
