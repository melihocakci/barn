#pragma once

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <functional>
#include <chrono>
#include <memory>
#include <future>

#define ACTION_PARAMETERS entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world
#define ACTION_VARIABLES entity, registry, renderer, mixer, world

namespace barn {
	struct body {
		const b2BodyId id{};

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
			const_cast<b2BodyId&>(other.id) = {};
		}
	};

	template<typename T>
	struct asset {
		std::shared_ptr<std::shared_future<T*>> ptr{};
		T& operator *() {
			return *ptr->get();
		}
		T* operator->() {
			return ptr->get();
		}
		T* get() {
			return ptr->get();
		}
		operator bool() const {
			return ptr->get();
		}
	};

	using texture = barn::asset<SDL_Texture>;

	using audio = barn::asset<MIX_Audio>;

	struct sprite {
		barn::texture texture{};
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	struct animation {
		barn::texture texture{};
		std::vector<SDL_FRect> frames{};
		std::optional<float> width{};
		std::optional<float> height{};
		std::chrono::milliseconds frame_duration{};
		int loop_count{};
		int priority{};
		std::chrono::steady_clock::time_point last_frame_time{};
		int current_frame_index{};
	};

	struct keyboard {};

	using gamepad = std::unique_ptr<SDL_Gamepad, decltype(&SDL_CloseGamepad)>;

	struct keyboard_controls {
		SDL_Scancode up{};
		SDL_Scancode down{};
		SDL_Scancode left{};
		SDL_Scancode right{};
		SDL_Scancode skill1{};
		SDL_Scancode skill2{};
		SDL_Scancode skill3{};
		SDL_Scancode skill4{};
	};

	struct gamepad_controls {
		SDL_GamepadAxis axis_x{};
		SDL_GamepadAxis axis_y{};
		SDL_GamepadButton skill1{};
		SDL_GamepadButton skill2{};
		SDL_GamepadButton skill3{};
		SDL_GamepadButton skill4{};
	};

	struct assets {
		std::vector<barn::texture> textures{};
		std::vector<barn::audio> audios{};
	};

	using action = void(*)(ACTION_PARAMETERS);

	struct skill {
		std::chrono::milliseconds cooldown{};
		barn::action action = [](ACTION_PARAMETERS) {};
		std::chrono::steady_clock::time_point last_used_time{};
	};

	using skillset = std::array<skill, 4>;

	struct properties {
		int health = 1;
		int attack = 0;
		float speed = 0.f;
	};

	enum category : std::uint64_t {
		ALLY = 1,
		FOE = 1 << 1,
		ALLY_BULLET = 1 << 2,
		FOE_BULLET = 1 << 3,
		OBSTACLE = 1 << 4,
	};

	struct background {};
	struct bullet {};

	enum player {
		P1,
		P2,
		P3,
		P4,
		COUNT
	};
}
