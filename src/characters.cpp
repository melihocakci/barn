#include "characters.h"
#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <array>

const project_stable::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, entt::entity player_entity) {
	},
};

///
/// Hatsune Miku
///

static const sf::Texture miku_texture{ "assets/texture/miku.png" };

static const project_stable::skill green_onion_attack 
{
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		static const sf::Texture bullet_texture{ "assets/texture/green-onion.png" };

		auto [player_hitbox, player_stats] = reg.get<project_stable::hitbox, project_stable::stats>(player_entity);

		const entt::entity bullet_entity = reg.create();
		project_stable::sprite& bullet_sprite = reg.emplace<project_stable::sprite>(bullet_entity, bullet_texture);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());
		bullet_sprite.setPosition(player_hitbox.getPosition());

		project_stable::hitbox& bullet_hitbox = reg.emplace<project_stable::hitbox>(bullet_entity, 10);
		bullet_hitbox.setOrigin(bullet_hitbox.getGeometricCenter());
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<project_stable::stats>(bullet_entity, project_stable::stats{ .health = 1, .attack = player_stats.attack, .speed = 30 });

		reg.emplace<project_stable::trajectory>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });

		reg.emplace<project_stable::alignment>(bullet_entity, project_stable::alignment::PLAYER);
	},
};

static const project_stable::character hatsune_miku 
{
	.sprite{ miku_texture },
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
