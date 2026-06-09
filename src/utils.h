#pragma once

#include <box2d/box2d.h>

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
}
