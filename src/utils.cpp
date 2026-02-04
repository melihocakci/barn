#include "utils.h"

#include <SDL3_image/SDL_image.h>

barn::texture barn::get_texture(SDL_Renderer* renderer, const std::filesystem::path& asset) {
	static std::map<std::filesystem::path, barn::texture::weak_type> textures;

	auto it = textures.find(asset);
	if (it != textures.end()) {
		if (auto existing = it->second.lock()) {
			return existing; // Still in use
		}
	}

	const std::filesystem::path combined_path = "assets" / asset;

	// Load new texture
	barn::texture texture{
		IMG_LoadTexture(renderer, combined_path.generic_string().c_str()),
		SDL_DestroyTexture
	};

	if (!texture) {
		SDL_Log("Failed to load texture: %s, SDL_Error: %s", asset.generic_string().c_str(), SDL_GetError());
		return nullptr;
	}

	textures[asset] = texture; // Store weak_ptr
	return texture;
}
