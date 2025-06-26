#include "characters.h"
#include "components.h"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <array>

const mg::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, entt::entity player_entity) {
	},
};

///
/// Hatsune Miku
///

static const sf::Texture miku_texture{ "assets/texture/miku.png" };

static const mg::skill green_onion_attack 
{
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, entt::entity player_entity) {
		static const sf::Texture bullet_texture{ "assets/texture/green-onion.png" };

		auto [player_hitbox, player_stats] = reg.get<mg::hitbox, mg::stats>(player_entity);

		const entt::entity bullet_entity = reg.create();
		mg::sprite& bullet_sprite = reg.emplace<mg::sprite>(bullet_entity, bullet_texture);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());
		bullet_sprite.setPosition(player_hitbox.getPosition());

		mg::hitbox& bullet_hitbox = reg.emplace<mg::hitbox>(bullet_entity, 10);
		bullet_hitbox.setOrigin(bullet_hitbox.getGeometricCenter());
		bullet_hitbox.setPosition(player_hitbox.getPosition());

		reg.emplace<mg::stats>(bullet_entity, mg::stats{ .health = 1, .attack = player_stats.attack, .speed = 30 });

		reg.emplace<mg::trajectory>(bullet_entity, 10.f, sf::Vector2f{ 0, -1 });

		reg.emplace<mg::alignment>(bullet_entity, mg::alignment::PLAYER);
	},
};

static const mg::character hatsune_miku 
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

const std::array<mg::character, 1> mg::character_templates
{
	hatsune_miku,
};
