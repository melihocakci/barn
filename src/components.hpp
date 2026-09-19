#pragma once

#include "constants.hpp"
#include "skills.hpp"
#include "assets.hpp"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <chrono>
#include <memory>
#include <future>
#include <filesystem>
#include <array>
#include <chrono>

namespace barn {
	enum class skill_state {
		NONE,
		PRESSED,
		RELEASED,
		HOLDING
	};
}

namespace barn::component {
	struct body {
		b2BodyId id{};

		body(b2BodyId body_id) : id(body_id) {}
		~body() noexcept {
			if (b2Body_IsValid(id)) {
				if (b2Body_GetUserData(id)) {
					delete static_cast<entt::entity*>(b2Body_GetUserData(id));
				}
				b2DestroyBody(id);
			}
		}
		body(const body&) = delete;
		body& operator=(const body&) = delete;
		body(body&& other) noexcept : id(other.id) {
			other.id = {};
		}
	};

	struct input {
		float axis_x = 0.f;
		float axis_y = 0.f;
		std::array<bool, SKILLSET_SIZE> skill_pressed = { false };
		std::array<barn::skill_state, SKILLSET_SIZE> skill_state = { barn::skill_state::NONE };
	};

	struct sprite {
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
		barn::texture texture{};
	};
	
	struct animation {
		std::vector<SDL_FRect> frames{};
		std::optional<float> width{};
		std::optional<float> height{};
		std::chrono::milliseconds duration{ 1000 };
		barn::texture texture{};
		std::chrono::steady_clock::time_point start_time{};
	};

	struct animation_list {
		enum class type {
			IDLE,
			ATTACK
		};

		type current = type::IDLE;
		int loops = 0;

		std::optional<component::animation> idle{};
		std::optional<component::animation> attack{};
	};

	struct track {
		barn::audio audio{};
		std::unique_ptr<MIX_Track, decltype(&MIX_DestroyTrack)> track;
	};

	struct keyboard {};

	struct gamepad {
		SDL_JoystickID id{};
	};

	using skillset = std::array<skill, SKILLSET_SIZE>;

	struct base_properties {
		int health = 1;
		int attack = 0;
		int defense = 0;
		int collide_damage = 0;
		int speed = 1;
	};

	struct properties : public base_properties {};

	using transform = b2Transform;

	struct previous_transform : public transform {};

	enum class player {
		P1,
		P2,
		P3,
		P4,
		COUNT
	};

	enum class AI_code {
		CHASER,
	};

	struct enemy {};
	struct bullet {};
	struct obstacle {};
	struct background {};
}
