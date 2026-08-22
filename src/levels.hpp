#pragma once

#include "components.hpp"
#include "presets.hpp"

namespace barn {
	struct level {
		barn::entity_def background{};
		std::vector<barn::entity_def> elements{};
		std::vector<barn::entity_def> enemies{};
	};
}
