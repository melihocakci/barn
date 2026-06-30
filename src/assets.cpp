#include "assets.h"
#include "components.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

barn::texture barn::get_texture(SDL_Renderer* renderer, std::string_view path) {
	using element_t = SDL_Texture*;
	using future_t = std::shared_future<element_t>;
	using shared_ptr_t = std::shared_ptr<future_t>;
	using weak_ptr_t = shared_ptr_t::weak_type;

	static std::unordered_map<std::string_view, weak_ptr_t> textures{};

	const auto it = textures.find(path);
	if (it != textures.end()) {
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
			return IMG_Load(path.data());
		});

	*texture = std::async(std::launch::deferred,
		[renderer, surf_ftr = std::move(surface_ftr)]() mutable -> SDL_Texture*
		{
			SDL_Surface* surface = surf_ftr.get();
			if (!renderer || !surface) {
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_DestroySurface(surface);
			return texture;
		});

	textures[path] = texture;
	return { texture };
}

barn::audio barn::get_audio(std::string_view path) {
	using element_t = MIX_Audio*;
	using future_t = std::shared_future<element_t>;
	using shared_ptr_t = std::shared_ptr<future_t>;
	using weak_ptr_t = shared_ptr_t::weak_type;

	static std::unordered_map<std::string_view, weak_ptr_t> audios{};

	const auto it = audios.find(path);
	if (it != audios.end()) {
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
			return MIX_LoadAudio(nullptr, path.data(), false);
		});

	audios[path] = audio;
	return { audio };
}
