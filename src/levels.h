#pragma once

#include "components.h"
#include "presets.h"

namespace barn {
	struct level {
		barn::entity_def background{};
		std::vector<barn::entity_def> elements{};
		std::vector<barn::entity_def> enemies{};
	};
}
