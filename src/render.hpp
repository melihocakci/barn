#pragma once

#include "context.hpp"

#include <SDL3/SDL.h>

namespace barn {
	void start_render(SDL_Renderer* renderer);

	void end_render(SDL_Renderer* renderer);

	void fill_screen(SDL_Renderer* renderer, SDL_Texture* texture);

	std::tuple<float, int, int> calculate_scale_and_offset(SDL_Renderer* renderer);

	void draw_borders(SDL_Renderer* renderer, float scale, int offset_x, int offset_y);

	void draw_texture(
		SDL_Renderer* renderer,
		barn::texture texture,
		barn::component::transform transform,
		const SDL_FRect* src_rect = nullptr,
		std::optional<float> width = std::nullopt,
		std::optional<float> height = std::nullopt,
		float scale = 1.f,
		int offset_x = 0,
		int offset_y = 0
	);
}
