#pragma once

#include "context.hpp"
#include "components.hpp"

namespace barn {
	struct entity_def {
		std::optional<barn::body_def> body{};
		std::optional<barn::animation_def> idle_animation{};
		std::optional<barn::sprite_def> sprite{};
		std::optional<barn::track_def> track{};
		std::optional<barn::base_properties> properties{};
		std::optional<barn::skillset_def> skillset{};
		std::optional<component::keyboard> keyboard{};
		std::optional<component::gamepad> gamepad{};
		std::optional<component::transform> transform{};
		std::optional<component::AI_code> AI_code{};
		std::optional<component::player> player{};
		std::optional<component::enemy> enemy{};
		std::optional<component::bullet> bullet{};
		std::optional<component::obstacle> obstacle{};
		std::optional<component::background> background{};
	};

	entt::entity create_entity(entt::registry& registry, barn::context& context, const barn::entity_def& def);
}
