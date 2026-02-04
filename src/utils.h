#pragma once

#include "components.h"

#include <box2d/types.h>
#include <SDL3/SDL.h>

#include <filesystem>
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

	barn::texture get_texture(SDL_Renderer* renderer, const std::filesystem::path& asset);
}
