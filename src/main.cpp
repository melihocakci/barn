#include "scenes.h"
#include "constants.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <box2d/box2d.h>

struct sdl_guard {
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	sdl_guard() {
		bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_CreateWindowAndRenderer(PROJECT_NAME, 1920, 1080, SDL_WINDOW_FULLSCREEN, &window, &renderer);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_SetRenderVSync(renderer, 1);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_SetRenderLogicalPresentation(
			renderer,
			VIRTUAL_WIDTH_PIXELS,
			VIRTUAL_HEIGHT_PIXELS,
			SDL_LOGICAL_PRESENTATION_LETTERBOX
		);
		if (!success) throw std::runtime_error(SDL_GetError());
	}
	~sdl_guard() {
		if (renderer) SDL_DestroyRenderer(renderer);
		if (window) SDL_DestroyWindow(window);
		SDL_Quit();
	}
};

struct b2_guard {
	b2WorldId world_id{};
	b2_guard() {
		b2WorldDef world_def = b2DefaultWorldDef();
		world_def.gravity = b2Vec2{ 0.0f, 0.0f };
		world_def.workerCount = 4;
		world_id = b2CreateWorld(&world_def);
	}
	~b2_guard() {
		b2DestroyWorld(world_id);
	}
};

int main(int argc, char* argv[]) {
	sdl_guard sdl_guard{};
	b2_guard b2_guard{};

	return barn::main_menu(sdl_guard.window, sdl_guard.renderer, b2_guard.world_id);
}
