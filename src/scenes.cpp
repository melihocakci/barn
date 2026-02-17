#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "definitions.h"
#include "constants.h"
#include "factories.h"
#include "config.h"
#include "utils.h"
#include "assets.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

int barn::main_menu(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world_id) {
	return barn::combat_scene(window, renderer, mixer, world_id);
}

int barn::combat_scene(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world) {
	entt::registry registry;

	const auto bliss_texture = get_texture(renderer, textures::bliss);
	entt::entity background = registry.create();
	registry.emplace<barn::sprite>(background, bliss_texture);
	registry.emplace<barn::background>(background);

	create_borders(registry, world);

	barn::audio bg_music = get_audio(audios::kasane_territory);
	MIX_Track* track = MIX_CreateTrack(mixer);
	MIX_SetTrackAudio(track, bg_music.get());
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	MIX_PlayTrack(track, props);

	barn::player_def player_def = character_templates[0];
	player_def.body.def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS / 4 };
	entt::entity player_entity = create_player(registry, renderer, mixer, world, player_def);
	registry.emplace<player>(player_entity, player::P1);
	registry.emplace<keyboard>(player_entity);
	//int count;
	//SDL_JoystickID* ids = SDL_GetGamepads(&count);
	//registry.emplace<gamepad>(player_entity, SDL_OpenGamepad(ids[0]), SDL_CloseGamepad);
	//SDL_free(ids);

	barn::enemy_def enemy_def = enemy_templates[0];
	enemy_def.body.def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS * 3 / 4 };
	create_enemy(registry, renderer, world, enemy_def);

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();

	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				return 0;
			}
		}

		draw_system(registry, renderer);

		Uint64 currentTicks = SDL_GetTicks();
		float deltaTime = (currentTicks - prevTicks) / 1000.0f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		while (accumulator >= PHYSICS_TIMESTEP) {
			if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) {
				keyboard_system(registry, renderer, mixer, world);
				gamepad_system(registry, renderer, mixer, world);
			}

			physics_system(registry, world);
			accumulator -= PHYSICS_TIMESTEP;

			action_system(registry, renderer, mixer, world);
		}
	}

	return 0;
}
