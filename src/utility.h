#pragma once

#include "constants.h"

#include <box2d/types.h>
#include <SFML/System.hpp>

#include <math.h>


inline b2Vec2 operator/(b2Vec2 left, float right) {
	return { left.x / right, left.y / right };
}

inline float length(b2Vec2 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline b2Vec2 normalize(b2Vec2 vec) {
	return vec / length(vec);
}

inline sf::Vector2f to_pixels(b2Vec2 vec) {
	return { vec.x * PIXELS_PER_METER, VIRTUAL_HEIGHT_PIXELS - vec.y * PIXELS_PER_METER };
}

inline b2Vec2 to_meters(sf::Vector2f vec) {
	return { vec.x / PIXELS_PER_METER, VIRTUAL_HEIGHT_METERS - vec.y / PIXELS_PER_METER };
}
