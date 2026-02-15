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

	barn::texture get_texture(SDL_Renderer* renderer, std::string_view path);

	barn::audio get_audio(std::string_view path);
}
