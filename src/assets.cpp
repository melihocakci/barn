#include "assets.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <unordered_map>
#include <filesystem>

barn::texture barn::get_texture(barn::context& context, const std::filesystem::path& path) {
	using element_t = SDL_Texture*;
	using future_t = std::shared_future<element_t>;
	using shared_ptr_t = std::shared_ptr<future_t>;

	const auto it = context.textures.find(path);
	if (it != context.textures.end()) {
		if (auto existing = it->second.lock()) {
			return { existing };
		}
	}

	shared_ptr_t texture{
		new future_t{},
		[](future_t* future)
		{
			if (future->get()) {
				SDL_DestroyTexture(future->get());
			}
			delete future;
		}
	};

	std::future<SDL_Surface*> surface_ftr = std::async(std::launch::async,
		[path]() -> SDL_Surface*
		{
			return IMG_Load(path.generic_string().c_str());
		});

	*texture = std::async(std::launch::deferred,
		[&context, surf_ftr = std::move(surface_ftr)]() mutable -> SDL_Texture*
		{
			SDL_Surface* surface = surf_ftr.get();
			if (!context.renderer || !surface) {
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(context.renderer, surface);
			SDL_DestroySurface(surface);
			return texture;
		});

	context.textures[path] = texture;
	return { texture };
}

barn::audio barn::get_audio(barn::context& context, const std::filesystem::path& path) {
	using element_t = MIX_Audio*;
	using future_t = std::shared_future<element_t>;
	using shared_ptr_t = std::shared_ptr<future_t>;

	const auto it = context.audios.find(path);
	if (it != context.audios.end()) {
		if (auto existing = it->second.lock()) {
			return { existing };
		}
	}

	shared_ptr_t audio{
		new future_t{},
		[](future_t* future)
		{
			if (future->get()) {
				MIX_DestroyAudio(future->get());
			}
			delete future;
		}
	};

	*audio = std::async(std::launch::async,
		[path]() -> MIX_Audio*
		{
			return MIX_LoadAudio(nullptr, path.generic_string().c_str(), false);
		});

	context.audios[path] = audio;
	return { audio };
}
