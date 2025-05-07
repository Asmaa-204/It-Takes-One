#pragma once

#include <components/component.hpp>
#include <components/mesh-renderer.hpp>
#include <entities/entity.hpp>
#include <glm/glm.hpp>
#include <iostream>

namespace our
{

    class EnemyAI : public Component
    {
    public:
        // Deserialize the component from JSON
        void deserialize(const nlohmann::json &data) override {}

        static std::string getID() { return "EnemyAI"; }
        virtual std::string getid() { return "EnemyAI"; }
    };

}  // namespace our
