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

static entt::entity create_borders(FACTORY_PARAMETERS) {
	using namespace barn;

	barn::body_def body_def{};
	body_def.def.type = b2_staticBody;
	body_def.def.position = { 0, 0 };

	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.categoryBits = barn::category::OBSTACLE;

	constexpr float half_width = 2.f;

	b2Polygon top_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS + half_width }, b2Rot_identity);
	b2Polygon bottom_rect = b2MakeOffsetBox(VIRTUAL_WIDTH_METERS, half_width, { VIRTUAL_WIDTH_METERS / 2, -half_width }, b2Rot_identity);
	b2Polygon left_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { -half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);
	b2Polygon right_rect = b2MakeOffsetBox(half_width, VIRTUAL_HEIGHT_METERS, { VIRTUAL_WIDTH_METERS + half_width, VIRTUAL_HEIGHT_METERS / 2 }, b2Rot_identity);

	body_def.polygons = {
		{ shape_def, top_rect },
		{ shape_def, bottom_rect },
		{ shape_def, left_rect },
		{ shape_def, right_rect }
	};

	barn::entity_def entity_def{
		.body = body_def,
		.obstacle = component::obstacle{},
	};

	return create_entity(FACTORY_VARIABLES, entity_def);
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

	barn::entity_def character_preset = character_presets[0];
	character_preset.body->def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS / 4 };
	character_preset.player = component::player::P1;
	entt::entity player_entity = create_entity(FACTORY_VARIABLES, character_preset);
	registry.emplace<component::keyboard>(player_entity);

	barn::entity_def enemy_preset = enemy_presets[0];
	enemy_preset.body->def.position = { VIRTUAL_WIDTH_METERS / 2, VIRTUAL_HEIGHT_METERS * 3 / 4 };
	create_entity(FACTORY_VARIABLES, enemy_preset);

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();

	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				return 0;
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_ADDED) {
				for (auto [entity, player] : registry.view<component::player>().each()) {
					if (!registry.any_of<component::gamepad>(entity)) {
						registry.emplace<component::gamepad>(player_entity, SDL_OpenGamepad(event.gdevice.which), SDL_CloseGamepad);
						break;
					}
				}
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED) {
				for (auto [entity, gamepad] : registry.view<component::gamepad>().each()) {
					if (SDL_GetGamepadID(gamepad.get()) == event.gdevice.which) {
						registry.remove<component::gamepad>(entity);
						break;
					}
				}
			}
		}

		const Uint64 currentTicks = SDL_GetTicks();
		const float deltaTime = (currentTicks - prevTicks) / 1000.0f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		while (accumulator >= PHYSICS_TIMESTEP) {
			property_system(registry);

			if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) {
				input_system(registry, renderer, mixer, world);
			}

			AI_system(registry, renderer, mixer, world);

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
