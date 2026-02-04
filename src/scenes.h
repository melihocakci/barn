#pragma once

#include "components.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace barn {
	int main_menu(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world_id);

	int combat_scene(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world_id);
}
