#include "settings.h"

#include <glaze/json.hpp>

glz::error_ctx barn::load_settings(barn::settings& settings) {
	return glz::read_file_json(settings, "settings.json", std::string{});
}

glz::error_ctx barn::save_settings(const barn::settings& settings) {
	return glz::write_file_json(settings, "settings.json", std::string{});
}

void barn::apply_settings(const barn::settings& settings, SDL_Renderer* renderer, MIX_Mixer* mixer) {
	MIX_SetMixerGain(mixer, settings.master_volume / 100.0f);
}
