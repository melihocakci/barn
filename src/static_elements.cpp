#include "static_elements.h"
#include "components.h"

#include <entt/entt.hpp>

#include <array>

const sf::Texture barn::texture::miku{ "assets/texture/miku.png" };
const sf::Texture barn::texture::pearto{ "assets/texture/pearto.png" };
const sf::Texture barn::texture::green_onion{ "assets/texture/green-onion.png" };
const sf::Texture barn::texture::bliss{ "assets/texture/bliss.jpg" };

const barn::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, entt::entity player_entity) {
	},
};

///
/// Characters
///

static const barn::skill green_onion_attack
{
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		auto [player_hitbox, player_stats] = reg.get<barn::hitbox, barn::properties>(player_entity);

		const entt::entity bullet_entity = reg.create();
		barn::sprite& bullet_sprite = reg.emplace<barn::sprite>(bullet_entity, barn::texture::green_onion);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());
		bullet_sprite.setPosition(player_hitbox.getPosition());

		barn::hitbox& bullet_hitbox = reg.emplace<barn::hitbox>(bullet_entity, sf::Vector2f{ 10, 10 });
		bullet_hitbox.setOrigin(bullet_hitbox.getGeometricCenter());
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<barn::properties>(bullet_entity, barn::properties{.health = 1, .attack = player_stats.attack, .speed = 30 });

		reg.emplace<barn::trajectory>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });

		reg.emplace<barn::type>(bullet_entity, barn::type::PROJECTILE);

		reg.emplace<barn::alignment>(bullet_entity, barn::alignment::ALLY);
	},
};

static const barn::character hatsune_miku
{
	.sprite{ barn::texture::miku },
	.hitbox{{ 20.f, 20.f }},
	.stats{
		.health = 1000,
		.attack = 10,
		.speed = 20,
	},
	.skillset{
		green_onion_attack,
		none,
		none,
		none,
	}
};


const std::array<barn::character, 1> barn::character_templates
{
	hatsune_miku,
};


///
/// Enemies
///

static const barn::enemy pearto_enemy
{
	.sprite{ barn::texture::pearto },
	.hitbox{{ 60.f, 60.f }},
	.stats
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.action = [](entt::registry& reg, entt::entity entity)
	{
		auto [enemy_hitbox, enemy_sprite, enemy_stats] = reg.get<barn::hitbox, barn::sprite, barn::properties>(entity);
		sf::Vector2f closest_player_position{ -100000.f, -100000.f };

		for (auto [entity, _, player_hitbox] : reg.view<barn::skillset, barn::hitbox>().each()) {
			if ((enemy_hitbox.getPosition() - closest_player_position).length() >
				(enemy_hitbox.getPosition() - player_hitbox.getPosition()).length())
			{
				closest_player_position = player_hitbox.getPosition();
			}
		}

		const sf::Vector2f distance = closest_player_position - enemy_hitbox.getPosition();

		if (distance.length() == 0) {
			return;
		}

		enemy_hitbox.move(distance.normalized() * enemy_stats.speed);
		enemy_sprite.setPosition(enemy_hitbox.getPosition());
	}
};


const std::array<barn::enemy, 1> barn::enemy_templates
{
	pearto_enemy,
};
