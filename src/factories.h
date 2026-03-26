#pragma once

#include "components.h"

#define FACTORY_PARAMETERS entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world
#define FACTORY_VARIABLES registry, renderer, mixer, world

namespace barn {
	struct entity_def {
		std::optional<barn::body_def> body{};
		std::optional<barn::animation_def> idle_animation{};
		std::optional<barn::base_properties> properties{};
		std::optional<barn::skillset_def> skillset{};
		std::optional<component::action> action{};
		std::optional<component::player> player{};
		std::optional<component::enemy> enemy{};
		std::optional<component::bullet> bullet{};
		std::optional<component::obstacle> obstacle{};
		std::optional<component::background> background{};
	};

	entt::entity create_entity(FACTORY_PARAMETERS, const barn::entity_def& def);
}
