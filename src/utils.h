#pragma once

#include "components.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <math.h>

namespace barn {
	inline b2Vec2 operator/(b2Vec2 left, float right) {
		return { left.x / right, left.y / right };
	}

	inline float length(b2Vec2 vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y);
	}

	inline b2Vec2 normalize(b2Vec2 vec) {
		return vec / length(vec);
	}

	template<typename asset_t, typename loader_t>
		requires ((std::is_same_v<asset_t, SDL_Texture>&& std::is_same_v<loader_t, SDL_Renderer>) || (std::is_same_v<asset_t, MIX_Audio> && std::is_same_v<loader_t, MIX_Mixer>))
	barn::asset<asset_t> get_asset(loader_t* loader, std::string_view path);

	inline barn::texture get_texture(SDL_Renderer* renderer, std::string_view path) {
		return get_asset<SDL_Texture>(renderer, path);
	}

	inline barn::audio get_audio(MIX_Mixer* mixer, std::string_view path) {
		return get_asset<MIX_Audio>(mixer, path);
	}
}
