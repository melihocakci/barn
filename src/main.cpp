#include "scenes.h"
#include "constants.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>

struct sdl_guard {
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	MIX_Mixer* mixer{};
	sdl_guard() {
		bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = MIX_Init();
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_CreateWindowAndRenderer(
			barn::PROJECT_NAME,
			barn::VIRTUAL_WIDTH_PIXELS, barn::VIRTUAL_HEIGHT_PIXELS,
			SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS,
			&window, &renderer);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_SetRenderVSync(renderer, 1);
		if (!success) throw std::runtime_error(SDL_GetError());

		success = SDL_SetRenderLogicalPresentation(
			renderer,
			barn::VIRTUAL_WIDTH_PIXELS,
			barn::VIRTUAL_HEIGHT_PIXELS,
			SDL_LOGICAL_PRESENTATION_LETTERBOX
		);
		if (!success) throw std::runtime_error(SDL_GetError());

		mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!mixer) throw std::runtime_error(SDL_GetError());
	}
	~sdl_guard() {
		if (mixer) MIX_DestroyMixer(mixer);
		if (renderer) SDL_DestroyRenderer(renderer);
		if (window) SDL_DestroyWindow(window);
		MIX_Quit();
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

	return barn::main_menu(sdl_guard.window, sdl_guard.renderer, sdl_guard.mixer, b2_guard.world_id);
}
