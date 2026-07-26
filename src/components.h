#pragma once

#include "constants.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <chrono>
#include <memory>
#include <future>
#include <filesystem>

namespace barn {
	struct circle_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Circle circle{};
	};

	struct polygon_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Polygon polygon{};
	};

	struct body_def {
		b2BodyDef def = b2DefaultBodyDef();
		std::vector<circle_def> circles{};
		std::vector<polygon_def> polygons{};
	};

	enum category : std::uint64_t {
		ALLY = 1,
		ENEMY = 1 << 1,
		ALLY_BULLET = 1 << 2,
		ENEMY_BULLET = 1 << 3,
		OBSTACLE = 1 << 4,
	};

	struct base_properties {
		int health = 1;
		int attack = 0;
		int defense = 0;
		int collide_damage = 0;
		int speed = 1;
	};

	template<typename T>
	struct asset {
		std::shared_ptr<std::shared_future<T*>> ptr{};

		T& operator *() {
			return *ptr->get();
		}
		const T& operator *() const {
			return *ptr->get();
		}
		T* operator->() {
			return ptr->get();
		}
		const T* operator->() const {
			return ptr->get();
		}
		T* get() {
			return ptr->get();
		}
		const T* get() const {
			return ptr->get();
		}
		operator bool() const {
			return ptr->get();
		}
	};

	using texture = barn::asset<SDL_Texture>;
	using audio = barn::asset<MIX_Audio>;

	using asset_def = std::filesystem::path;

	struct sprite_def {
		barn::asset_def texture{};
		std::optional<SDL_FRect> src_rect{};
		std::optional<float> width{};
		std::optional<float> height{};
	};

	using namespace std::chrono_literals;

	struct animation_def {
		barn::asset_def texture{};
		std::vector<SDL_FRect> frames{};
		std::optional<float> width{};
		std::optional<float> height{};
		std::chrono::milliseconds duration = 1000ms;
	};

	struct assets_def {
		std::vector<barn::asset_def> textures{};
		std::vector<barn::asset_def> audios{};
	};

	struct assets {
		std::vector<barn::texture> textures{};
		std::vector<barn::audio> audios{};
	};

	enum class skill_code {
		NONE,
		GREEN_ONION,
	};

	struct skill_def {
		barn::skill_code code{};
		barn::assets_def assets{};
		std::chrono::milliseconds cooldown{};
	};

	using skillset_def = std::array<skill_def, SKILLSET_SIZE>;

	struct skill {
		barn::skill_def def{};
		barn::assets assets{};
		std::chrono::steady_clock::time_point last_used_time{};
	};

	using gamepad = std::unique_ptr<SDL_Gamepad, decltype(&SDL_CloseGamepad)>;
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
		bool skills[SKILLSET_SIZE] = { false };
	};

	struct sprite {
		barn::sprite_def def{};
		barn::texture texture{};
	};

	struct animation {
		barn::animation_def def{};
		barn::texture texture{};
		std::chrono::steady_clock::time_point start_time{};
	};

	struct idle_animation : public animation {};

	struct keyboard {};

	struct gamepad {
		SDL_JoystickID id{};
	};

	using skillset = std::array<skill, SKILLSET_SIZE>;

	struct properties : public base_properties {
		base_properties base{};
	};

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
		NONE,
		CHASER,
	};

	struct enemy {};
	struct bullet {};
	struct obstacle {};
}
