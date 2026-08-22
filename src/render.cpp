#include "render.hpp"
#include "assets.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

void barn::start_render(SDL_Renderer* renderer) {
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void barn::end_render(SDL_Renderer* renderer) {
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}

void barn::fill_screen(SDL_Renderer* renderer, SDL_Texture* texture) {
	float tex_w, tex_h;
	SDL_GetTextureSize(texture, &tex_w, &tex_h);

	int screen_w, screen_h;
	SDL_GetRenderOutputSize(renderer, &screen_w, &screen_h);

	float scale = std::max(static_cast<float>(screen_w) / tex_w, static_cast<float>(screen_h) / tex_h);
	float visible_w = screen_w / scale;
	float visible_h = screen_h / scale;
	float crop_x = (tex_w - visible_w) * 0.5f;
	float crop_y = (tex_h - visible_h) * 0.5f;

	SDL_FRect src = { crop_x, crop_y, visible_w, visible_h };
	SDL_RenderTexture(renderer, texture, &src, nullptr);
}

std::tuple<float, int, int> barn::calculate_scale_and_offset(SDL_Renderer* renderer) {
	int window_w, window_h;
	SDL_GetCurrentRenderOutputSize(renderer, &window_w, &window_h);
	float scale = std::min(static_cast<float>(window_w) / barn::VIRTUAL_WIDTH_PIXELS, static_cast<float>(window_h) / barn::VIRTUAL_HEIGHT_PIXELS);
	int offset_x = (window_w - scale * barn::VIRTUAL_WIDTH_PIXELS) / 2;
	int offset_y = (window_h - scale * barn::VIRTUAL_HEIGHT_PIXELS) / 2;
	return std::make_tuple(scale, offset_x, offset_y);
}

void barn::draw_borders(SDL_Renderer* renderer, float scale, int offset_x, int offset_y) {
	const int thickness = 2; 
	const Uint8 r = 0, g = 0, b = 0, a = 80;
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	float offset_x_f = static_cast<float>(offset_x);
	float offset_y_f = static_cast<float>(offset_y);
	SDL_FRect rects[4] = {
		{ offset_x_f, offset_y_f, barn::VIRTUAL_WIDTH_PIXELS * scale, thickness },														// top
		{ offset_x_f, offset_y_f + barn::VIRTUAL_HEIGHT_PIXELS * scale - thickness, barn::VIRTUAL_WIDTH_PIXELS * scale, thickness },	// bottom
		{ offset_x_f, offset_y_f, thickness, barn::VIRTUAL_HEIGHT_PIXELS * scale },														// left
		{ offset_x_f + barn::VIRTUAL_WIDTH_PIXELS * scale - thickness, offset_y_f, thickness, barn::VIRTUAL_HEIGHT_PIXELS * scale }		// right
	};
	SDL_RenderFillRects(renderer, rects, 4);
}

void barn::draw_texture(
	SDL_Renderer* renderer,
	barn::texture texture,
	barn::component::transform transform,
	const SDL_FRect* src_rect,
	std::optional<float> width,
	std::optional<float> height,
	float scale,
	int offset_x,
	int offset_y
) {
	if (!texture) return;

	const float texture_aspect_ratio = static_cast<float>(texture->w) / texture->h;

	float dest_width, dest_height;
	if (width && height) {
		dest_width = *width;
		dest_height = *height;
	}
	else if (!width && height) {
		dest_width = *height * texture_aspect_ratio;
		dest_height = *height;
	}
	else if (width && !height) {
		dest_width = *width;
		dest_height = *width / texture_aspect_ratio;
	}
	else {
		dest_width = texture->w;
		dest_height = texture->h;
	}

	const SDL_FRect dest_rect = {
		static_cast<float>(offset_x) / scale + transform.p.x * barn::PIXELS_PER_METER - dest_width / 2,
		static_cast<float>(offset_y) / scale + barn::VIRTUAL_HEIGHT_PIXELS - transform.p.y * barn::PIXELS_PER_METER - dest_height / 2,
		dest_width,
		dest_height
	};

	float prev_scale_x, prev_scale_y;
	SDL_GetRenderScale(renderer, &prev_scale_x, &prev_scale_y);
	SDL_SetRenderScale(renderer, scale, scale);

	SDL_RenderTextureRotated(
		renderer,
		texture.get(),
		src_rect,
		&dest_rect,
		b2Rot_GetAngle(transform.q) * 360 / B2_PI,
		nullptr,
		SDL_FLIP_NONE
	);

	SDL_SetRenderScale(renderer, prev_scale_x, prev_scale_y);
}
