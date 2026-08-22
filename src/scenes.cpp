#include "scenes.h"
#include "systems.h"
#include "presets.h"
#include "constants.h"
#include "factories.h"
#include "utils.h"
#include "assets.h"
#include "levels.h"
#include "interface.h"
#include "render.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <imgui_impl_sdl3.h>


static entt::entity create_borders(entt::registry& registry, barn::context& context) {
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

	return create_entity(registry, context, entity_def);
}

barn::menu_action barn::home_scene(barn::context& context) {
	std::vector<barn::menu> menu_stack{ barn::menu::MAIN_MENU };
	barn::texture background_texture = barn::get_texture(context.renderer, barn::textures::bliss);

	while (!context.exit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);

			if (event.type == SDL_EVENT_QUIT) {
				context.exit = true;
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_ADDED) {
				context.gamepads.emplace(event.gdevice.which, barn::gamepad{ SDL_OpenGamepad(event.gdevice.which), SDL_CloseGamepad });
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED) {
				context.gamepads.erase(event.gdevice.which);
			}
		}

		start_render(context.renderer);

		fill_screen(context.renderer, background_texture.get());

		barn::menu_action result = draw_menu(context, menu_stack);

		end_render(context.renderer);

		if (result != barn::menu_action::NONE) {
			return result;
		}
	}

	return barn::menu_action::NONE;
}

std::optional<barn::session> barn::lobby_scene(barn::context& context) {
	barn::session session{
	.players = {
		barn::character_presets[0],
	},
	.level{
		.bg_music = barn::audios::kasane_territory,
		.bg_texture = barn::textures::bliss,
		.elements{
		},
		.enemies{
			barn::enemy_presets[0],
		}
	}
	};
	session.players[0].player = barn::component::player::P1;
	if (!context.gamepads.empty()) {
		session.players[0].gamepad = { .id = context.gamepads.begin()->first };
	}
	session.players[0].keyboard = barn::component::keyboard{};
	session.players[0].transform = barn::component::transform{ { barn::VIRTUAL_WIDTH_METERS / 2.f, barn::VIRTUAL_HEIGHT_METERS / 4.f }, b2Rot_identity };

	return session;
}

void barn::combat_scene(barn::context& context, barn::session& session) {
	barn::audio bg_music = barn::get_audio(session.level.bg_music);
	barn::texture bg_texture = barn::get_texture(context.renderer, session.level.bg_texture);
	MIX_Track* track = MIX_CreateTrack(context.mixer);
	MIX_SetTrackAudio(track, bg_music.get());
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	MIX_PlayTrack(track, props);

	entt::registry registry{};

	create_borders(registry, context);

	for (const barn::entity_def& element_def : session.level.elements) {
		create_entity(registry, context, element_def);
	}

	for (const barn::entity_def& player_def : session.players) {
		create_entity(registry, context, player_def);
	}

	for (const barn::entity_def& enemy_def : session.level.enemies) {
		create_entity(registry, context, enemy_def);
	}

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();

	std::vector<barn::menu> menu_stack{};
	while (!context.exit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);

			if (event.type == SDL_EVENT_QUIT) {
				context.exit = true;
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_ADDED) {
				context.gamepads.emplace(event.gdevice.which, barn::gamepad{ SDL_OpenGamepad(event.gdevice.which), SDL_CloseGamepad });
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED) {
				context.gamepads.erase(event.gdevice.which);
			}
			else if ((event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_Scancode::SDL_SCANCODE_ESCAPE)
				|| (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN && event.gbutton.button == SDL_GAMEPAD_BUTTON_START))
			{
				if (menu_stack.empty()) {
					menu_stack.push_back(barn::menu::PAUSE_MENU);
				}
			}
		}

		const Uint64 currentTicks = SDL_GetTicks();
		const float deltaTime = menu_stack.empty() ? (currentTicks - prevTicks) / 1000.0f : 0.f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		while (accumulator >= barn::PHYSICS_TIMESTEP) {
			barn::property_system(registry);

			if (SDL_GetWindowFlags(context.window) & SDL_WINDOW_INPUT_FOCUS) {
				barn::keyboard_system(registry, context);
				barn::gamepad_system(registry, context);
			}

			barn::input_system(registry, context);

			barn::AI_system(registry, context);

			barn::body_system(registry, context);
			accumulator -= barn::PHYSICS_TIMESTEP;
		}

		barn::start_render(context.renderer);
		barn::fill_screen(context.renderer, bg_texture.get());
		auto [scale, offset_x, offset_y] = barn::calculate_scale_and_offset(context.renderer);
		barn::draw_borders(context.renderer, scale, offset_x, offset_y);
		const float alpha = accumulator / barn::PHYSICS_TIMESTEP;
		barn::sprite_system(registry, context, alpha, scale, offset_x, offset_y);
		barn::animation_system(registry, context, alpha, scale, offset_x, offset_y);
		barn::draw_ui(context, registry);
		barn::menu_action result = barn::draw_menu(context, menu_stack);
		barn::end_render(context.renderer);

		if (result == barn::menu_action::EXIT) {
			break;
		}
	}
}
