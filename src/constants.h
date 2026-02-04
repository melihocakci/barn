#pragma once

constexpr const char* PROJECT_NAME = "Barn";
constexpr float VIRTUAL_WIDTH_PIXELS = 1920.f;
constexpr float VIRTUAL_HEIGHT_PIXELS = 1080.f;
constexpr float VIRTUAL_WIDTH_METERS = 32.f;
constexpr float VIRTUAL_HEIGHT_METERS = 18.f;
constexpr float PIXELS_PER_METER = VIRTUAL_HEIGHT_PIXELS / VIRTUAL_HEIGHT_METERS;
constexpr int FRAME_RATE = 60;
constexpr float TIME_STEP = 1.f / FRAME_RATE;
constexpr int SUB_STEP_COUNT = 4;
