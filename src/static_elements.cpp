#include "static_elements.h"
#include "components.h"

#include <entt/entt.hpp>

#include <array>

const sf::Texture project_stable::texture::miku{ "assets/texture/miku.png" };
const sf::Texture project_stable::texture::pearto{ "assets/texture/pearto.png" };
const sf::Texture project_stable::texture::green_onion{ "assets/texture/green-onion.png" };
const sf::Texture project_stable::texture::bliss{ "assets/texture/bliss.jpg" };

const project_stable::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, entt::entity player_entity) {
	},
};

///
/// Characters
///

static const project_stable::skill green_onion_attack
{
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		auto [player_hitbox, player_stats] = reg.get<project_stable::hitbox, project_stable::stats>(player_entity);

		const entt::entity bullet_entity = reg.create();
		project_stable::sprite& bullet_sprite = reg.emplace<project_stable::sprite>(bullet_entity, project_stable::texture::green_onion);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());
		bullet_sprite.setPosition(player_hitbox.getPosition());

		project_stable::hitbox& bullet_hitbox = reg.emplace<project_stable::hitbox>(bullet_entity, 10);
		bullet_hitbox.setOrigin(bullet_hitbox.getGeometricCenter());
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<project_stable::stats>(bullet_entity, project_stable::stats{.health = 1, .attack = player_stats.attack, .speed = 30 });

		reg.emplace<project_stable::trajectory>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });

		reg.emplace<project_stable::alignment>(bullet_entity, project_stable::alignment::PLAYER);
	},
};

static const project_stable::character hatsune_miku
{
	.sprite{ project_stable::texture::miku },
	.hitbox{ 20.f },
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


const std::array<project_stable::character, 1> project_stable::character_templates
{
	hatsune_miku,
};


///
/// Enemies
///

static const project_stable::enemy pearto_enemy
{
	.sprite{ project_stable::texture::pearto },
	.hitbox{ 60.f },
	.stats
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.action = [](entt::registry& reg, entt::entity entity) {
		auto [enemy_hitbox, enemy_sprite, enemy_stats] = reg.get<project_stable::hitbox, project_stable::sprite, project_stable::stats>(entity);
		sf::Vector2f closest_player_position{ -100000.f, -100000.f };

		for (auto [entity, _, player_hitbox] : reg.view<project_stable::skillset, project_stable::hitbox>().each()) {
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


const std::array<project_stable::enemy, 1> project_stable::enemy_templates
{
	pearto_enemy,
};
