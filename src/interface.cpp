#include "interface.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>


constexpr ImGuiWindowFlags window_flags =
	ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_AlwaysAutoResize;

constexpr ImVec2 button_size{ 200.0f, 40.0f };

static void main_menu(barn::context& context, std::vector<barn::menu_state>& menu_stack) {
	ImGui::Begin("Main Menu", nullptr, window_flags);

	ImGui::SetWindowFontScale(1.8f);
	float title_width = ImGui::CalcTextSize("BARN").x;
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
	ImGui::Text("BARN");
	ImGui::SetWindowFontScale(1.0f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Start", button_size)) {
		menu_stack.push_back(barn::menu_state::START_GAME);
	}

	ImGui::Spacing();

	if (ImGui::Button("Settings", button_size)) {
		menu_stack.push_back(barn::menu_state::SETTINGS_MENU);
	}

	ImGui::Spacing();

	if (ImGui::Button("Exit", button_size)) {
		menu_stack.push_back(barn::menu_state::EXIT);
	}

	ImGui::End();
}

static void settings_menu(barn::context& context, std::vector<barn::menu_state>& menu_stack) {
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Audio Settings");
	ImGui::Spacing();

	ImGui::PushItemWidth(200.0f);

	if (ImGui::SliderFloat("Volume", &context.settings.master_volume, 0.0f, 100.0f, "%.0f%%")) {
		barn::apply_settings(context.settings, context.renderer, context.mixer);
	}

	ImGui::PopItemWidth();

	ImGui::Spacing();

	ImGui::Checkbox("Show FPS", &context.settings.show_fps);

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Back", ImVec2(200.0f, 40.0f))) {
		menu_stack.pop_back();
	}

	ImGui::End();
}

static void pause_menu(std::vector<barn::menu_state>& menu_stack) {
	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImU32 tint_color = IM_COL32(0, 0, 0, 150);
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ 0.0f, 0.0f }, screen_size, tint_color);

	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.5f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Main Menu", nullptr, window_flags);

	if (ImGui::Button("Resume", button_size)) {
		menu_stack.pop_back();
	}

	ImGui::Spacing();

	if (ImGui::Button("Settings", button_size)) {
		menu_stack.push_back(barn::menu_state::SETTINGS_MENU);
	}

	ImGui::Spacing();

	if (ImGui::Button("Exit", button_size)) {
		menu_stack.push_back(barn::menu_state::EXIT);
	}

	ImGui::End();
}

void barn::draw_menu(barn::context& context, std::vector<barn::menu_state>& menu_stack) {
	if (menu_stack.empty()) {
		return;
	}

	switch (menu_stack.back()) {
	case barn::menu_state::MAIN_MENU:
		return main_menu(context, menu_stack);
	case barn::menu_state::PAUSE_MENU:
		return pause_menu(menu_stack);
	case barn::menu_state::SETTINGS_MENU:
		return settings_menu(context, menu_stack);
	}
}

void barn::draw_ui(barn::context& context, entt::registry& registry) {
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

