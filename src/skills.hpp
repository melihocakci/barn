#pragma once

#include "context.hpp"
#include "assets.hpp"

#include <entt/entt.hpp>

#include <variant>
#include <string_view>
#include <chrono>

namespace barn::skills {
	using namespace std::chrono;

	struct skill_interface {
		virtual std::string_view name() const = 0;
		virtual std::string_view description() const = 0;
		virtual std::chrono::nanoseconds static_cooldown() const = 0;
		virtual std::chrono::nanoseconds base_cooldown() const = 0;

		virtual void initialize(barn::context& c) = 0;
		virtual void update(barn::context& c, entt::registry& r, entt::entity e) = 0;
		virtual void pressed(barn::context& c, entt::registry& r, entt::entity e) = 0;
		virtual void holding(barn::context& c, entt::registry& r, entt::entity e) = 0;
		virtual void released(barn::context& c, entt::registry& r, entt::entity e) = 0;

		std::chrono::steady_clock::time_point cooldown_end{};

		void start_cooldown(entt::registry& registry, entt::entity entity) {
			cooldown_end = std::chrono::steady_clock::now() + static_cooldown() + base_cooldown();
		}

		bool on_cooldown() const {
			return std::chrono::steady_clock::now() < cooldown_end;
		}
	};

	struct green_onion : public skill_interface {
		std::string_view name() const override { return "Green Onion"; }
		std::string_view description() const override { return "Shoots a green onion projectile."; }
		std::chrono::nanoseconds static_cooldown() const override { return 0ms; }
		std::chrono::nanoseconds base_cooldown() const override { return 1000ms; }

		void initialize(barn::context& c) override;
		void update(barn::context& c, entt::registry& r, entt::entity e) override {};
		void pressed(barn::context& c, entt::registry& r, entt::entity e) override { holding(c, r, e); }
		void holding(barn::context& c, entt::registry& r, entt::entity e) override;
		void released(barn::context& c, entt::registry& r, entt::entity e) override {};

		barn::texture onion{};
		barn::audio weii{};
	};
}

namespace barn {
	using skill = std::variant<skills::green_onion>;
}
