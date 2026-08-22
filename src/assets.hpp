#pragma once

#include "components.hpp"

#include <SDL3/SDL.h>

#include <filesystem>

namespace barn::textures {
	constexpr std::string_view bliss = "assets/texture/bliss.jpg";
	constexpr std::string_view green_onion = "assets/texture/green-onion.png";
	constexpr std::string_view miku = "assets/texture/miku.png";
	constexpr std::string_view miku_animation = "assets/texture/miku_sheet.png";
	constexpr std::string_view pearto = "assets/texture/pearto.png";
	constexpr std::string_view clovers = "assets/texture/clovers.jpg";
	constexpr std::string_view clouds = "assets/texture/clouds.jpg";
}

namespace barn::audios {
	constexpr std::string_view kasane_territory = "assets/audio/kasane_territory.ogg";
	constexpr std::string_view weiii = "assets/audio/weiii-daitaku-helios.ogg";
}

namespace barn {
	barn::texture get_texture(SDL_Renderer* renderer, const std::filesystem::path& path);

	barn::audio get_audio(const std::filesystem::path& path);
}
