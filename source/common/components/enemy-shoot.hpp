#pragma once

#include <components/component.hpp>
#include <components/mesh-renderer.hpp>
#include <entities/entity.hpp>
#include <glm/glm.hpp>
#include <iostream>

namespace our {

    class EnemyShoot : public Component {
    public:
        bool shoot = false;
        // TODO: Add a direction for the bullet to got to, maybe also add a
        // starting position or get it from the entity

        // Deserialize the component from JSON
        void deserialize(const nlohmann::json &data) override {}

        static std::string getID() { return "EnemyShoot"; }
        virtual std::string getid() { return "EnemyShoot"; }
    };

}  // namespace our
