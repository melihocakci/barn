#pragma once

#include "settings.hpp"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>

#include <unordered_map>
#include <future>

namespace barn {
	using gamepad = std::unique_ptr<SDL_Gamepad, decltype(&SDL_CloseGamepad)>;

	using texture_ptr = std::shared_ptr<std::shared_future<SDL_Texture*>>::weak_type;
	using audio_ptr = std::shared_ptr<std::shared_future<MIX_Audio*>>::weak_type;

	struct context {
		SDL_Window* const window{};
		SDL_Renderer* const renderer{};
		MIX_Mixer* const mixer{};
		b2WorldId const world_id{};
		barn::settings settings{};
		std::unordered_map<std::filesystem::path, texture_ptr> textures{};
		std::unordered_map<std::filesystem::path, audio_ptr> audios{};
		std::unordered_map<SDL_JoystickID, barn::gamepad> gamepads{};
		bool exit = false;
		bool paused = false;
	};
}
