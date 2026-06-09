#include "settings.h"

#include <glaze/json.hpp>

barn::settings barn::load_settings() {
	barn::settings settings{};
	[[maybe_unused]] auto error = glz::read_file_json(settings, "settings.json", std::string{});
	return settings;
}

void barn::save_settings(const barn::settings& settings) {
	[[maybe_unused]] auto error = glz::write_file_json(settings, "settings.json", std::string{});
}

void barn::apply_settings(const barn::settings& settings, SDL_Renderer* renderer, MIX_Mixer* mixer) {
	MIX_SetMixerGain(mixer, settings.master_volume / 100.0f);
}
