#pragma once

namespace barn {
	constexpr const char* PROJECT_NAME = "Barn";
	constexpr int VIRTUAL_WIDTH_PIXELS = 1920;
	constexpr int VIRTUAL_HEIGHT_PIXELS = 1080;
	constexpr int VIRTUAL_WIDTH_METERS = 16;
	constexpr int VIRTUAL_HEIGHT_METERS = 9;
	constexpr int PIXELS_PER_METER = VIRTUAL_HEIGHT_PIXELS / VIRTUAL_HEIGHT_METERS;
	constexpr int BOX2D_SUB_STEP_COUNT = 4;
	constexpr float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	constexpr int SKILLSET_SIZE = 4;
	constexpr int PLAYER_COUNT = 4;
}
