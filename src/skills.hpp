#pragma once

#include "context.hpp"

#include <entt/entt.hpp>

#include <variant>
#include <string_view>
#include <chrono>

namespace barn::skills {
	using namespace std::chrono;

	struct skill_interface {
		virtual std::string_view name() const = 0;
		virtual std::string_view description() const = 0;
		virtual std::chrono::milliseconds static_cooldown() const = 0;
		virtual std::chrono::milliseconds base_cooldown() const = 0;

		virtual void initialize(barn::context& context) = 0;
		virtual void key_down(barn::context& context, entt::registry& registry, entt::entity entity) = 0;
		virtual void key_up(barn::context& context, entt::registry& registry, entt::entity entity) = 0;

		std::chrono::steady_clock::time_point cooldown_end{};

		void start_cooldown(entt::registry& registry, entt::entity entity) {
			cooldown_end = std::chrono::steady_clock::now() + static_cooldown() + base_cooldown();
		}

		bool on_cooldown() {
			return std::chrono::steady_clock::now() < cooldown_end;
		}
	};

	struct green_onion : public skill_interface {
		std::string_view name() const override { return "Green Onion"; }
		std::string_view description() const override { return "Shoots a green onion projectile."; }
		std::chrono::milliseconds static_cooldown() const override { return 0ms; }
		std::chrono::milliseconds base_cooldown() const override { return 250ms; }

		void initialize(barn::context& context) override;
		void key_down(barn::context& context, entt::registry& registry, entt::entity entity) override;
		void key_up(barn::context& context, entt::registry& registry, entt::entity entity) override;
	};
}

namespace barn {
	using skill = std::variant<skills::green_onion>;
}
