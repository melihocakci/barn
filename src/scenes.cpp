#include "scenes.h"
#include "components.h"
#include "systems.h"
#include "presets.h"
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

	entt::entity background = registry.create();
	barn::sprite_def background_def{
		.texture = textures::bliss,
	};
	const auto bliss_texture = get_texture(renderer, textures::bliss);
	registry.emplace<component::sprite>(background, background_def, get_texture(renderer, background_def.texture));
	registry.emplace<component::background>(background);

	create_borders(FACTORY_VARIABLES);

	barn::audio bg_music = get_audio(audios::kasane_territory);
	MIX_Track* track = MIX_CreateTrack(mixer);
	MIX_SetTrackAudio(track, bg_music.get());
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	MIX_PlayTrack(track, props);

	barn::character_preset character_preset = character_presets[0];
	character_preset.body.def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS / 4 };
	entt::entity player_entity = create_player(registry, renderer, mixer, world, character_preset);
	registry.emplace<component::player>(player_entity, component::player::P1);
	registry.emplace<component::keyboard>(player_entity);
	//int count;
	//SDL_JoystickID* ids = SDL_GetGamepads(&count);
	//registry.emplace<gamepad>(player_entity, SDL_OpenGamepad(ids[0]), SDL_CloseGamepad);
	//SDL_free(ids);

	barn::enemy_preset enemy_preset = enemy_presets[0];
	enemy_preset.body.def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS * 3 / 4 };
	create_enemy(FACTORY_VARIABLES, enemy_preset);

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();

	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				return 0;
			}
		}

		const Uint64 currentTicks = SDL_GetTicks();
		const float deltaTime = (currentTicks - prevTicks) / 1000.0f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		while (accumulator >= PHYSICS_TIMESTEP) {
			property_system(registry);

			if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) {
				keyboard_system(registry, renderer, mixer, world);
				gamepad_system(registry, renderer, mixer, world);
			}

			action_system(registry, renderer, mixer, world);

			physics_system(registry, world);
			accumulator -= PHYSICS_TIMESTEP;
		}

		// compute alpha for interpolation (clamp to [0,1])
		const float alpha = std::clamp(accumulator / PHYSICS_TIMESTEP, 0.0f, 1.0f);

		// draw using interpolated positions
		draw_system(registry, renderer, alpha);
	}

	return 0;
}
