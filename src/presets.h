#pragma once

#include "components.h"

#include <array>

namespace barn {
	struct character_preset {
		barn::body_def body{};
		barn::animation_def idle_animation{};
		barn::base_properties properties{};
		barn::skillset_def skillset{};
	};

	struct enemy_preset {
		barn::body_def body{};
		barn::animation_def idle_animation{};
		barn::base_properties properties{};
		barn::action action = empty_action;
	};

	struct bullet_preset {
		barn::body_def body{};
		barn::animation_def idle_animation{};
		barn::base_properties properties{};
	};

	extern const std::array<character_preset, 1> character_presets;

	extern const std::array<enemy_preset, 1> enemy_presets;
}
