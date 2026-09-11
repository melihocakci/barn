#pragma once

#include "context.hpp"

#include <entt/entt.hpp>

#include <string>
#include <variant>

struct skill {
	std::string name;
	int level;
	std::string description;

	virtual void execute(barn::context& context, entt::registry& registry, entt::entity entity) = 0;
};

struct green_onion_skill : public skill {
	std::string name = "Green Onion";
	int level = 1;
	std::string description = "Shoots a green onion that deals damage to enemies.";
	void execute(barn::context& context, entt::registry& registry, entt::entity entity) override;
};

namespace barn::component {
	using skill_new = std::variant<green_onion_skill>;
}
