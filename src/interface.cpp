#include "interface.hpp"

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
constexpr float menu_width = 350.0f;

// Helper function to center text properly
static void centered_text(const char* text, bool large = false) {
	if (large) ImGui::SetWindowFontScale(1.8f);

	ImGui::NewLine();
	float text_width = ImGui::CalcTextSize(text).x;
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
	ImGui::Text(text);

	if (large) ImGui::SetWindowFontScale(1.0f);
}

static barn::menu_action main_menu(std::vector<barn::menu>& menu_stack) {
	barn::menu_action result = barn::menu_action::NONE;

	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.5f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(menu_width, 0), ImVec2(menu_width, FLT_MAX));
	ImGui::Begin("Main Menu", nullptr, window_flags);

	centered_text("BARN", true);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Center buttons
	float button_offset = (menu_width - button_size.x) * 0.5f;
	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Start", button_size)) {
		menu_stack.pop_back();
		result = barn::menu_action::START_GAME;
	}

	ImGui::Spacing();

	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Settings", button_size)) {
		menu_stack.push_back(barn::menu::SETTINGS_MENU);
	}

	ImGui::Spacing();

	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Exit", button_size)) {
		menu_stack.pop_back();
		result = barn::menu_action::EXIT;
	}

	ImGui::End();

	return result;
}

static barn::menu_action settings_menu(barn::context& context, std::vector<barn::menu>& menu_stack) {
	barn::menu_action result = barn::menu_action::NONE;

	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImU32 tint_color = IM_COL32(0, 0, 0, 150);
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ 0.0f, 0.0f }, screen_size, tint_color);

	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.5f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(menu_width, 0), ImVec2(menu_width, FLT_MAX));

	ImGui::Begin("Settings", nullptr, window_flags);

	centered_text("Settings", true);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

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

	float button_offset = (menu_width - button_size.x) * 0.5f;
	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Back", button_size) || ImGui::Shortcut(ImGuiKey_Escape) || ImGui::Shortcut(ImGuiKey_GamepadFaceRight) || ImGui::Shortcut(ImGuiKey_GamepadStart)) {
		menu_stack.pop_back();
	}

	ImGui::End();

	return result;
}

static barn::menu_action pause_menu(std::vector<barn::menu>& menu_stack) {
	barn::menu_action result = barn::menu_action::NONE;

	ImVec2 screen_size = ImGui::GetIO().DisplaySize;
	ImU32 tint_color = IM_COL32(0, 0, 0, 150);
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ 0.0f, 0.0f }, screen_size, tint_color);

	ImVec2 center{ screen_size.x * 0.5f, screen_size.y * 0.5f };
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(menu_width, 0), ImVec2(menu_width, FLT_MAX));

	ImGui::Begin("Pause Menu", nullptr, window_flags);

	centered_text("PAUSED", true);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float button_offset = (menu_width - button_size.x) * 0.5f;
	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Resume", button_size) || ImGui::Shortcut(ImGuiKey_Escape) || ImGui::Shortcut(ImGuiKey_GamepadFaceRight) || ImGui::Shortcut(ImGuiKey_GamepadStart)) {
		menu_stack.pop_back();
	}

	ImGui::Spacing();

	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Settings", button_size)) {
		menu_stack.push_back(barn::menu::SETTINGS_MENU);
	}

	ImGui::Spacing();

	ImGui::SetCursorPosX(button_offset);
	if (ImGui::Button("Exit", button_size)) {
		menu_stack.pop_back();
		result = barn::menu_action::EXIT;
	}

	ImGui::End();

	return result;
}

barn::menu_action barn::draw_menu(barn::context& context, std::vector<barn::menu>& menu_stack) {
	if (menu_stack.empty()) {
		return barn::menu_action::NONE;
	}

	barn::menu_action result = barn::menu_action::NONE;
	switch (menu_stack.back()) {
	case barn::menu::MAIN_MENU:
		result = main_menu(menu_stack);
		break;
	case barn::menu::PAUSE_MENU:
		result = pause_menu(menu_stack);
		break;
	case barn::menu::SETTINGS_MENU:
		result = settings_menu(context, menu_stack);
		break;
	}

	return menu_stack.empty() ? result : barn::menu_action::NONE;
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

