#pragma once

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <filesystem>
#include <future>

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
	constexpr std::string_view ocarine_of_time = "assets/audio/ocarine_of_time.ogg";
}

namespace barn {
	template<typename T>
	struct asset {
		std::shared_ptr<std::shared_future<T*>> ptr{};

		T& operator *() {
			return *ptr->get();
		}
		const T& operator *() const {
			return *ptr->get();
		}
		T* operator->() {
			return ptr->get();
		}
		const T* operator->() const {
			return ptr->get();
		}
		T* get() {
			return ptr->get();
		}
		const T* get() const {
			return ptr->get();
		}
		operator bool() const {
			return ptr->get();
		}
	};

	using texture = barn::asset<SDL_Texture>;
	using audio = barn::asset<MIX_Audio>;

	using asset_def = std::filesystem::path;
}

namespace barn {
	barn::texture get_texture(SDL_Renderer* renderer, const std::filesystem::path& path);

	barn::audio get_audio(const std::filesystem::path& path);
}
