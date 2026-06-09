#pragma once

#include "components.h"
#include "presets.h"

namespace barn {
	struct level {
		barn::asset_def bg_music{};
		std::vector<barn::entity_def> elements{};
		std::vector<barn::entity_def> enemies{};
	};
}
