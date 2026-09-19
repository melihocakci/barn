#pragma once

#include <chrono>

namespace barn {
	constexpr const char* PROJECT_NAME = "Barn";
	constexpr int VIRTUAL_WIDTH_PIXELS = 1440;
	constexpr int VIRTUAL_HEIGHT_PIXELS = 1080;
	constexpr int VIRTUAL_WIDTH_METERS = 16;
	constexpr int VIRTUAL_HEIGHT_METERS = 12;
	constexpr int PIXELS_PER_METER = VIRTUAL_HEIGHT_PIXELS / VIRTUAL_HEIGHT_METERS;
	constexpr int BOX2D_SUB_STEP_COUNT = 4;
	constexpr std::chrono::nanoseconds PHYSICS_TIMESTEP{ 1'000'000'000 / 64 }; // 64Hz, exactly representable in nanoseconds
	constexpr int SKILLSET_SIZE = 4;
	constexpr int PLAYER_COUNT = 4;
}
