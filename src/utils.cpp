#include "utils.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

barn::texture barn::get_texture(SDL_Renderer* renderer, std::string_view path) {
	static std::unordered_map<std::string_view, barn::texture::weak_type> textures{};

	auto it = textures.find(path);
	if (it != textures.end()) {
		if (auto existing = it->second.lock()) {
			return existing;
		}
	}

	barn::texture texture{
		IMG_LoadTexture(renderer, path.data()),
		SDL_DestroyTexture
	};

	if (!texture) {
		SDL_Log("Failed to load texture: %s, SDL_Error: %s", path.data(), SDL_GetError());
		return nullptr;
	}

	textures[path] = texture;
	return texture;
}

barn::audio barn::get_audio(MIX_Mixer* mixer, std::string_view path) {
	static std::unordered_map<std::string_view, barn::audio::weak_type> audios{};

	auto it = audios.find(path);
	if (it != audios.end()) {
		if (auto existing = it->second.lock()) {
			return existing;
		}
	}

	barn::audio audio{
		MIX_LoadAudio(mixer, path.data(), false),
		MIX_DestroyAudio
	};

	if (!audio) {
		SDL_Log("Failed to load audio: %s, SDL_Error: %s", path.data(), SDL_GetError());
		return nullptr;
	}

	audios[path] = audio;
	return audio;
}
