#include "context.h"
#include "scenes.h"
#include "constants.h"
#include "utils.h"
#include "settings.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

void show_error_and_exit(const std::string& message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), nullptr);
	throw std::runtime_error(message);
}

struct context_guard {
	barn::context context{};

	context_guard() {
		bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
		if (!success) {
			show_error_and_exit("Failed to initialize SDL: " + std::string(SDL_GetError()));
		};

		success = MIX_Init();
		if (!success) {
			show_error_and_exit("Failed to initialize SDL_mixer: " + std::string(SDL_GetError()));
		};

		success = SDL_CreateWindowAndRenderer(
			barn::PROJECT_NAME,
			barn::VIRTUAL_WIDTH_PIXELS, barn::VIRTUAL_HEIGHT_PIXELS,
			SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS,
			&const_cast<SDL_Window*&>(context.window), &const_cast<SDL_Renderer*&>(context.renderer));
		if (!success) {
			show_error_and_exit("Failed to create SDL window and renderer");
		};

		success = SDL_SetRenderVSync(context.renderer, 1);
		if (!success) {
			show_error_and_exit("Failed to set VSync");
		};

		const_cast<MIX_Mixer*&>(context.mixer) = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!context.mixer) {
			show_error_and_exit("Failed to create mixer device");
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

		ImGui::StyleColorsDark();

		success = ImGui_ImplSDL3_InitForSDLRenderer(context.window, context.renderer);
		if (!success) {
			show_error_and_exit("Failed to initialize ImGui SDL3 implementation");
		}
		success = ImGui_ImplSDLRenderer3_Init(context.renderer);
		if (!success) {
			show_error_and_exit("Failed to initialize ImGui SDLRenderer3 implementation");
		}

		b2WorldDef world_def = b2DefaultWorldDef();
		world_def.gravity = b2Vec2{ 0.0f, 0.0f };
		world_def.workerCount = 4;
		const_cast<b2WorldId&>(context.world_id) = b2CreateWorld(&world_def);

		auto error = barn::load_settings(context.settings);
		if (error) {
			show_error_and_exit(std::string(error.custom_error_message));
		}
	}

	~context_guard() {
		barn::save_settings(context.settings);

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

	barn::apply_settings(guard.context.settings, guard.context.renderer, guard.context.mixer);

	return barn::main_menu(guard.context);
}
