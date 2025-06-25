#include "enemies.h"
#include "components.h"

static const sf::Texture pearto_texture{ "assets/texture/pearto.png" };

const std::array<mg::enemy, 1> mg::enemy_templates = {
	mg::enemy{
		.sprite = mg::sprite{ pearto_texture },
		.hitbox = mg::hitbox{ 20.f },
		.stats = {
			.health = 100,
			.speed = 10.f,
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
	}
};
