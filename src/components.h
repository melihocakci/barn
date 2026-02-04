#pragma once

#include <SDL3/SDL.h>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <functional>
#include <chrono>
#include <memory>

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

	using texture = std::shared_ptr<SDL_Texture>;

	struct sprite {
		barn::texture texture;
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	using action = std::function<void(entt::registry&, SDL_Renderer*, b2WorldId, entt::entity)>;

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

	struct skill {
		mutable std::chrono::steady_clock::time_point last_used_time{};
		std::chrono::milliseconds cooldown{};
		barn::action action = [](entt::registry&, SDL_Renderer*, b2WorldId, entt::entity) {};

		void operator()(entt::registry& reg, SDL_Renderer* renderer, b2WorldId world, entt::entity ent) const {
			using namespace std::chrono;

			auto current_time = steady_clock::now();
			auto time_span = duration_cast<milliseconds>(current_time - last_used_time);

			if (time_span >= cooldown) {
				action(reg, renderer, world, ent);
				last_used_time = current_time;
			}
		}
	};

	struct skillset {
		barn::skill skill1{};
		barn::skill skill2{};
		barn::skill skill3{};
		barn::skill skill4{};
	};

	struct properties {
		int health = 1;
		int attack = 0;
		float speed = 0;
	};

	enum category : std::uint64_t {
		ALLY = 1,
		FOE = 2,
		ALLY_BULLET = 4,
		FOE_BULLET = 8,
		OBSTACLE = 16,
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
