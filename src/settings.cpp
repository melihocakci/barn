#include "settings.hpp"

#include <glaze/json.hpp>
#include <filesystem>

glz::error_ctx barn::load_settings(barn::settings& settings) {
	if (!std::filesystem::exists("settings.json")) {
		save_settings(barn::settings{});
	}
	return glz::read_file_json(settings, "settings.json", std::string{});
}

glz::error_ctx barn::save_settings(const barn::settings& settings) {
	return glz::write_file_json(settings, "settings.json", std::string{});
}

void barn::apply_settings(const barn::settings& settings, [[maybe_unused]] SDL_Renderer* renderer, MIX_Mixer* mixer) {
	MIX_SetMixerGain(mixer, powf(settings.master_volume / 100.0f, 2.0f));
}
