#include "assets.h"

barn::texture barn::load_texture(const std::filesystem::path& asset) {
	static std::map<std::filesystem::path, std::weak_ptr<sf::Texture>> textures;

	auto it = textures.find(asset);
	if (it != textures.end()) {
		if (auto existing = it->second.lock()) {
			return existing; // Still in use
		}
	}

	// Load new texture
	auto texture = std::make_shared<sf::Texture>(asset);
	textures[asset] = texture; // Store weak_ptr
	return texture;
}
