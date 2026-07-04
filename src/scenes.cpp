#include "scenes.h"
#include "systems.h"
#include "presets.h"
#include "constants.h"
#include "factories.h"
#include "utils.h"
#include "assets.h"
#include "levels.h"

#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

int barn::main_menu(barn::context& context) {
	barn::session session{
		.players = {
			character_presets[0],
		},
		.level{
			.bg_music = audios::kasane_territory,
			.bg_texture = textures::bliss,
			.elements{
			},
			.enemies{
				enemy_presets[0],
			}
		}
	};
	session.players[0].player = component::player::P1;

	return barn::combat_scene(context, session);
}

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

static void draw_ui(barn::context& context, entt::registry& registry) {
	constexpr ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_AlwaysAutoResize;

	if (context.settings.show_fps) {
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::Begin("FPS", nullptr, window_flags);
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.9f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Health", nullptr, window_flags);
	for (auto [entity, player, properties] : registry.view<barn::component::player, barn::component::properties>().each()) {
		float progress = static_cast<float>(properties.health) / properties.base.health;

		const ImVec4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
		const ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f };
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progress < 0.2f ? ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f } : ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

		ImGui::ProgressBar(progress, ImVec2{ screen_size.x * 0.5f, 20.0f }, std::format("HP: {}/{}", properties.health, properties.base.health).c_str());

		ImGui::PopStyleColor(2);
	}
	ImGui::End();
}

enum class game_state {
	COMBAT,
	MENU,
	OPTIONS,
	EXIT,
};

static void draw_menu(barn::context& context, game_state& current_menu) {
	if (current_menu == game_state::COMBAT) {
		return;
	}

	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImU32 tint_color = IM_COL32(0, 0, 0, 150);
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ 0.0f, 0.0f }, screen_size, tint_color);

	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.5f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	constexpr ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_AlwaysAutoResize;
	const ImVec2 button_size{ 200.0f, 40.0f };

	switch (current_menu) {
	case game_state::MENU:
		ImGui::Begin("Main Menu", nullptr, window_flags);

		if (ImGui::Button("Resume", button_size)) {
			current_menu = game_state::COMBAT;
		}

		ImGui::Spacing();

		if (ImGui::Button("Options", button_size)) {
			current_menu = game_state::OPTIONS;
		}

		ImGui::Spacing();

		if (ImGui::Button("Exit", button_size)) {
			current_menu = game_state::EXIT;
		}

		ImGui::End();
		break;
	case game_state::OPTIONS:
		// We use the same windowFlags so it looks identical to the main menu
		ImGui::Begin("Options", nullptr, window_flags);

		ImGui::Text("Audio Settings");
		ImGui::Spacing();

		// Set the width of the slider to match your buttons for a clean look
		ImGui::PushItemWidth(200.0f);

		// The slider modifies 'volume' directly. 
		// Ranges from 0.0f to 100.0f, and displays with a '%' sign.
		if (ImGui::SliderFloat("Volume", &context.settings.master_volume, 0.0f, 100.0f, "%.0f%%")) {
			barn::apply_settings(context.settings, context.renderer, context.mixer);
		}

		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Checkbox("Show FPS", &context.settings.show_fps);

		ImGui::Spacing();
		ImGui::Spacing();

		// Return to the main menu
		if (ImGui::Button("Back", button_size)) {
			current_menu = game_state::MENU;
		}

		ImGui::End();
		break;
	}
}

static bool enable_logical_presentation(SDL_Renderer* renderer) {
	return SDL_SetRenderLogicalPresentation(
		renderer,
		barn::VIRTUAL_WIDTH_PIXELS,
		barn::VIRTUAL_HEIGHT_PIXELS,
		SDL_LOGICAL_PRESENTATION_LETTERBOX
	);
}

static bool disable_logical_presentation(SDL_Renderer* renderer) {
	return SDL_SetRenderLogicalPresentation(renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
}

int barn::combat_scene(barn::context& context, barn::session& session) {
	barn::audio bg_music = barn::get_audio(session.level.bg_music);
	barn::texture bg_texture = barn::get_texture(context.renderer, session.level.bg_texture);
	MIX_Track* track = MIX_CreateTrack(context.mixer);
	MIX_SetTrackAudio(track, bg_music.get());
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	MIX_PlayTrack(track, props);

	entt::registry registry;

	create_borders(registry, context);

	for (auto element_def : session.level.elements) {
		entt::entity element_entity = create_entity(registry, context, element_def);
	}

	for (int i = 0; i < session.players.size(); ++i) {
		barn::entity_def player_def = session.players[i];
		player_def.body->def.position = { static_cast<float>(VIRTUAL_WIDTH_METERS) / (session.players.size() + 1) * (i + 1), VIRTUAL_HEIGHT_METERS / 4 };
		player_def.player = session.players[i].player;
		entt::entity player_entity = create_entity(registry, context, player_def);
		registry.emplace<component::keyboard>(player_entity);
	}

	for (auto enemy_def : session.level.enemies) {
		entt::entity enemy_entity = create_entity(registry, context, enemy_def);
	}

	float accumulator = 0.0f;
	Uint64 prevTicks = SDL_GetTicks();
	game_state state = game_state::COMBAT;

	barn::texture sidebar_texture = barn::get_texture(context.renderer, barn::textures::clovers);

	while (state != game_state::EXIT) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);

			if (event.type == SDL_EVENT_QUIT) {
				state = game_state::EXIT;
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_ADDED) {
				for (auto [entity, player] : registry.view<component::player>().each()) {
					//if (!registry.any_of<component::gamepad>(entity)) {
					//	registry.emplace<component::gamepad>(player_entity, SDL_OpenGamepad(event.gdevice.which), SDL_CloseGamepad);
					//	break;
					//}
				}
			}
			else if (event.type == SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED) {
				for (auto [entity, gamepad] : registry.view<component::gamepad>().each()) {
					//if (SDL_GetGamepadID(gamepad.get()) == event.gdevice.which) {
					//	registry.remove<component::gamepad>(entity);
					//	break;
					//}
				}
			}
			else if ((event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_Scancode::SDL_SCANCODE_ESCAPE)
				|| (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN && event.gbutton.button == SDL_GAMEPAD_BUTTON_BACK)) {
				state = state == game_state::COMBAT ? game_state::MENU : game_state::COMBAT;
			}
		}

		const Uint64 currentTicks = SDL_GetTicks();
		const float deltaTime = state == game_state::COMBAT ? (currentTicks - prevTicks) / 1000.0f : 0.f;
		prevTicks = currentTicks;
		accumulator += deltaTime;

		while (accumulator >= PHYSICS_TIMESTEP) {
			property_system(registry);

			if (SDL_GetWindowFlags(context.window) & SDL_WINDOW_INPUT_FOCUS) {
				keyboard_system(registry, context);
				gamepad_system(registry, context);
			}

			input_system(registry, context);

			AI_system(registry, context);

			body_system(registry, context);
			accumulator -= PHYSICS_TIMESTEP;
		}

		SDL_RenderClear(context.renderer);

		disable_logical_presentation(context.renderer);

		SDL_RenderTexture(context.renderer, sidebar_texture.get(), nullptr, nullptr);

		enable_logical_presentation(context.renderer);

		SDL_RenderTexture(context.renderer, bg_texture.get(), nullptr, nullptr);

		// compute alpha for interpolation (clamp to [0,1])
		const float alpha = std::clamp(accumulator / PHYSICS_TIMESTEP, 0.0f, 1.0f);
		sprite_system(registry, context, alpha);
		animation_system(registry, context, alpha);

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		draw_ui(context, registry);
		draw_menu(context, state);

		ImGui::Render();

		disable_logical_presentation(context.renderer);

		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), context.renderer);

		SDL_RenderPresent(context.renderer);
	}

	return 0;
}
