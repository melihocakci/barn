#include "types.h"
#include "scenes.h"
#include "constants.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

struct context_guard {
	barn::context context{};

	context_guard() {
		errno = 0;
		SDL_SetError("");

		bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
		if (!success) {
			errno = 1;
			return;
		};

		success = MIX_Init();
		if (!success) {
			errno = 2;
			return;
		};

		success = SDL_CreateWindowAndRenderer(
			barn::PROJECT_NAME,
			barn::VIRTUAL_WIDTH_PIXELS, barn::VIRTUAL_HEIGHT_PIXELS,
			SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS,
			&const_cast<SDL_Window*&>(context.window), &const_cast<SDL_Renderer*&>(context.renderer));
		if (!success) {
			errno = 3;
			return;
		};

		success = SDL_SetRenderVSync(context.renderer, 1);
		if (!success) {
			errno = 4;
			return;
		};

		const_cast<MIX_Mixer*&>(context.mixer) = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!context.mixer) {
			errno = 5;
			return;
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

		ImGui::StyleColorsDark();

		success = ImGui_ImplSDL3_InitForSDLRenderer(context.window, context.renderer);
		if (!success) {
			errno = 6;
			return;
		}
		success = ImGui_ImplSDLRenderer3_Init(context.renderer);
		if (!success) {
			errno = 7;
			return;
		}

		b2WorldDef world_def = b2DefaultWorldDef();
		world_def.gravity = b2Vec2{ 0.0f, 0.0f };
		world_def.workerCount = 4;
		const_cast<b2WorldId&>(context.world_id) = b2CreateWorld(&world_def);
	}

	~context_guard() {
		if (b2World_IsValid(context.world_id)) b2DestroyWorld(context.world_id);

		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		if (context.mixer) MIX_DestroyMixer(context.mixer);
		if (context.renderer) SDL_DestroyRenderer(context.renderer);
		if (context.window) SDL_DestroyWindow(context.window);
		MIX_Quit();
		SDL_Quit();
	}
};

int main(int argc, char* argv[]) {
	context_guard guard{};
	if (errno) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Initialization Error",
			("Failed to start game. Error: " + std::to_string(errno) + "\n" + SDL_GetError()).c_str(),
			nullptr
		);
		return errno;
	}

	return barn::main_menu(guard.context);
}
