#pragma once

#include "components.h"

namespace barn {
	int main_menu(SDL_Window* window, SDL_Renderer* renderer, b2WorldId world_id);

	int combat_scene(SDL_Window* window, SDL_Renderer* renderer, b2WorldId world_id);
}
