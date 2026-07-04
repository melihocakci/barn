#pragma once

#include "components.h"
#include "presets.h"

namespace barn {
	struct level {
		barn::asset_def bg_music{};
		barn::asset_def bg_texture{};
		std::vector<barn::entity_def> elements{};
		std::vector<barn::entity_def> enemies{};
	};
}
