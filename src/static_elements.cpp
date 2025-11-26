#include "static_elements.h"
#include "components.h"
#include "utility.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <array>

const sf::Texture barn::texture::miku{ "assets/texture/miku.png" };
const sf::Texture barn::texture::pearto{ "assets/texture/pearto.png" };
const sf::Texture barn::texture::green_onion{ "assets/texture/green-onion.png" };
const sf::Texture barn::texture::bliss{ "assets/texture/bliss.jpg" };

const barn::skill none = {
	.cooldown_time{},
	.action = [](entt::registry& reg, b2WorldId world, entt::entity player_entity) {
	},
};

///
/// Characters
///

static const barn::skill green_onion_attack
{
	.cooldown_time{ 250 },
	.action = [](entt::registry& reg, b2WorldId world_id, entt::entity player_entity) {
		auto [player_body, player_stats] = reg.get<barn::body, barn::properties>(player_entity);

		const entt::entity bullet_entity = reg.create();
		barn::sprite& bullet_sprite = reg.emplace<barn::sprite>(bullet_entity, barn::texture::green_onion);
		bullet_sprite.setOrigin(bullet_sprite.getLocalBounds().getCenter());

		b2BodyDef body_def = b2DefaultBodyDef();
		body_def.type = b2_dynamicBody;
		body_def.position = b2Body_GetPosition(player_body);
		b2BodyId body_id = b2CreateBody(world_id, &body_def);
		b2Polygon dynamic_box = b2MakeBox(1.0f, 1.0f);
		b2ShapeDef shape_def = b2DefaultShapeDef();
		shape_def.density = 1.0f;
		shape_def.material.friction = 0.3f;
		b2CreatePolygonShape(body_id, &shape_def, &dynamic_box);

		reg.emplace<barn::body>(bullet_entity, body_id);

		reg.emplace<barn::properties>(bullet_entity, barn::properties{.health = 1, .attack = player_stats.attack, .speed = 30 });

		reg.emplace<barn::type>(bullet_entity, barn::type::PROJECTILE);

		reg.emplace<barn::alignment>(bullet_entity, barn::alignment::ALLY);
	},
};

static const barn::character hatsune_miku
{
	.sprite{ barn::texture::miku },
	.size{ 1.f, 2.f },
	.properties{
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
	.size{ 2.f, 2.f },
	.stats
	{
		.health = 100,
		.attack = 10,
		.speed = 5,
	},
	.action = [](entt::registry& reg, b2WorldId world_id, entt::entity entity)
	{
		auto [enemy_body, enemy_stats] = reg.get<barn::body, barn::properties>(entity);
		b2Vec2 enemy_position = b2Body_GetPosition(enemy_body);

		float shortest_distance = -1.f;
		b2Vec2 closest_target{};

		for (auto [entity, _, player_body] : reg.view<barn::skillset, barn::body>().each()) {
			const b2Vec2 player_position = b2Body_GetPosition(player_body);

			float distance = length(enemy_position - player_position);

			if (distance < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance;
				closest_target = player_position;
			}
		}

		if (shortest_distance < 0) {
			b2Body_SetLinearVelocity(enemy_body, { 0,0 });
			return;
		}

		b2Vec2 vel = normalize(closest_target - enemy_position) * enemy_stats.speed;
		b2Body_SetLinearVelocity(enemy_body, vel);
	}
};


const std::array<barn::enemy, 1> barn::enemy_templates
{
	pearto_enemy,
};
