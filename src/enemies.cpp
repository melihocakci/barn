#include "enemies.h"
#include "components.h"

///
/// Pearto
///

static const sf::Texture pearto_texture{ "assets/texture/pearto.png" };

static const mg::enemy pearto_enemy
{
	.sprite{ pearto_texture },
	.hitbox{ 60.f },
	.stats
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.action = [](entt::registry& reg, entt::entity entity) {
		auto [enemy_hitbox, enemy_sprite, enemy_stats] = reg.get<mg::hitbox, mg::sprite, mg::stats>(entity);
		sf::Vector2f closest_player_position{ -100000.f, -100000.f };

		for (auto [entity, _, player_hitbox] : reg.view<mg::skillset, mg::hitbox>().each()) {
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


const std::array<mg::enemy, 1> mg::enemy_templates
{
	pearto_enemy,
};
