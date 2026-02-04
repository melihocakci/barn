#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "definitions.h"
#include "constants.h"
#include "factories.h"
#include "config.h"
#include "utils.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

int barn::main_menu(SDL_Window* window, SDL_Renderer* renderer, b2WorldId world_id) {
	return combat_scene(window, renderer, world_id);
}

int barn::combat_scene(SDL_Window* window, SDL_Renderer* renderer, b2WorldId world_id) {
	entt::registry registry;

	const auto bliss_texture = get_texture(renderer, "texture/bliss.jpg");
	entt::entity background = registry.create();
	registry.emplace<barn::sprite>(background, bliss_texture);
	registry.emplace<barn::background>(background);

	create_borders(registry, world_id);

	barn::player_def player_def = character_templates[0];
	player_def.body.body.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS / 4 };
	entt::entity player_entity = create_player(registry, renderer, world_id, player_def);
	registry.emplace<player>(player_entity, player::P1);
	//registry.emplace<keyboard>(player_entity);
	int count;
	SDL_JoystickID* ids = SDL_GetGamepads(&count);
	registry.emplace<gamepad>(player_entity, SDL_OpenGamepad(ids[0]), SDL_CloseGamepad);
	SDL_free(ids);

	barn::enemy_def enemy_def = enemy_templates[0];
	enemy_def.body.body.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS * 3 / 4 };
	create_enemy(registry, renderer, world_id, enemy_def);

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();

	while (true) {
		// --- Event Handling ---
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				return 0;
			}
		}

		// --- Timing ---
		Uint64 currentTicks = SDL_GetTicks();
		float deltaTime = (currentTicks - prevTicks) / 1000.0f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		// --- Physics ---
		while (accumulator >= PHYSICS_TIMESTEP) {
			b2World_Step(world_id, PHYSICS_TIMESTEP, BOX2D_SUB_STEP_COUNT);
			accumulator -= PHYSICS_TIMESTEP;
		}

		action_system(registry, renderer, world_id);

		// --- Input ---
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) {
			keyboard_system(registry, renderer, world_id);
			gamepad_system(registry, renderer, world_id);
		}

		// --- Rendering ---
		sprite_system(registry, renderer);
	}

	return 0;
}
