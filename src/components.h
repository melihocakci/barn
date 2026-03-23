#pragma once

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include <chrono>
#include <memory>
#include <future>

#define ACTION_PARAMETERS entt::entity entity, entt::registry& registry, SDL_Renderer* renderer, MIX_Mixer* mixer, b2WorldId world, barn::action_state state
#define ACTION_VARIABLES entity, registry, renderer, mixer, world

namespace barn {
	struct circle_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Circle circle{};
	};

	struct polygon_def {
		b2ShapeDef def = b2DefaultShapeDef();
		b2Polygon polygon{};
	};

	enum category : std::uint64_t {
		ALLY = 1,
		FOE = 1 << 1,
		ALLY_BULLET = 1 << 2,
		FOE_BULLET = 1 << 3,
		OBSTACLE = 1 << 4,
	};

	struct body_def {
		b2BodyDef def = b2DefaultBodyDef();
		std::vector<circle_def> circles{};
		std::vector<polygon_def> polygons{};
	};

	using asset_def = std::string_view;

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

	struct base_properties {
		int health = 1;
		int attack = 0;
		int defense = 0;
		int collide_damage = 0;
		int speed = 1;
	};

	enum action_state {
		ACTION_INITIALIZE = 1,
		ACTION_RUN = 2,
		ACTION_CLEANUP = 3,
	};

	using action = void(*)(ACTION_PARAMETERS);

	constexpr action empty_action = [](ACTION_PARAMETERS) {};

	struct skill_def {
		std::chrono::milliseconds cooldown{};
		barn::action action = empty_action;
	};

	using skillset_def = std::array<skill_def, 4>;

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

	struct skill {
		barn::skill_def def{};
		std::chrono::steady_clock::time_point last_used_time{};
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

	using gamepad = std::unique_ptr<SDL_Gamepad, decltype(&SDL_CloseGamepad)>;

	using action = barn::action;

	using skillset = std::array<skill, sizeof(skillset_def) / sizeof(skillset_def::value_type)>;

	struct properties {
		barn::base_properties base{};
		int health = 1;
		int attack = 0;
		int defense = 0;
		int collide_damage = 0;
		int speed = 0;
	};

	using transform = b2Transform;

	enum class player {
		P1,
		P2,
		P3,
		P4,
		COUNT
	};

	struct enemy {};
	struct bullet {};
	struct obstacle {};
	struct background {};
}
