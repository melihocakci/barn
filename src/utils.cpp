#include "utils.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <future>

template barn::asset<SDL_Texture> barn::get_asset(SDL_Renderer*, std::string_view);
template barn::asset<MIX_Audio> barn::get_asset(MIX_Mixer*, std::string_view);

template<typename asset_t, typename loader_t>
	requires((std::is_same_v<asset_t, SDL_Texture>&& std::is_same_v<loader_t, SDL_Renderer>) || (std::is_same_v<asset_t, MIX_Audio> && std::is_same_v<loader_t, MIX_Mixer>))
barn::asset<asset_t> barn::get_asset(loader_t* loader, std::string_view path) {
	using element_t = asset_t*;
	using future_t = std::shared_future<element_t>;
	using shared_ptr_t = std::shared_ptr<future_t>;
	using weak_ptr_t = std::weak_ptr<future_t>;

	static std::unordered_map<std::string_view, weak_ptr_t> assets{};

	const auto it = assets.find(path);
	if (it != assets.end()) {
		if (auto existing = it->second.lock()) {
			return { existing };
		}
	}

	shared_ptr_t asset_ptr;
	if constexpr (std::is_same_v<asset_t, SDL_Texture>) {
		asset_ptr = shared_ptr_t{
			new future_t{},
			[](future_t* future)
			{
				if (future->get()) {
					SDL_DestroyTexture(future->get());
				}
			}
		};

		*asset_ptr = std::async(std::launch::async,
			[loader, path]()
			{
				return IMG_LoadTexture(loader, path.data());
			});
	}
	else if constexpr (std::is_same_v<asset_t, MIX_Audio>) {
		asset_ptr = shared_ptr_t{
			new future_t{},
			[](future_t* future)
			{
				if (future->get()) {
					MIX_DestroyAudio(future->get());
				}
			}
		};

		*asset_ptr = std::async(std::launch::async,
			[loader, path]()
			{
				return MIX_LoadAudio(loader, path.data(), false);
			});
	}
	else {
		static_assert("Unsupported asset type");
	}

	assets[path] = asset_ptr;
	return { asset_ptr };
}
