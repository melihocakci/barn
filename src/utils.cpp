#include "utils.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

barn::texture barn::get_texture(SDL_Renderer* renderer, const std::filesystem::path& asset) {
	static std::map<std::filesystem::path, barn::texture::weak_type> textures;

	auto it = textures.find(asset);
	if (it != textures.end()) {
		if (auto existing = it->second.lock()) {
			return existing;
		}
	}

	const std::filesystem::path combined_path = "assets" / asset;

	barn::texture texture{
		IMG_LoadTexture(renderer, combined_path.generic_string().c_str()),
		SDL_DestroyTexture
	};

	if (!texture) {
		SDL_Log("Failed to load texture: %s, SDL_Error: %s", asset.generic_string().c_str(), SDL_GetError());
		return nullptr;
	}

	textures[asset] = texture;
	return texture;
}

barn::audio barn::get_audio(MIX_Mixer* mixer, const std::filesystem::path& asset) {
	static std::map<std::filesystem::path, barn::audio::weak_type> audios;

	auto it = audios.find(asset);
	if (it != audios.end()) {
		if (auto existing = it->second.lock()) {
			return existing;
		}
	}

	const std::filesystem::path combined_path = "assets" / asset;

	barn::audio audio{
		MIX_LoadAudio(mixer, combined_path.generic_string().c_str(), false),
		MIX_DestroyAudio
	};

	if (!audio) {
		SDL_Log("Failed to load audio: %s, SDL_Error: %s", asset.generic_string().c_str(), SDL_GetError());
		return nullptr;
	}

	audios[asset] = audio;
	return audio;
}
