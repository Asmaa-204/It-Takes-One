#pragma once

#include <components/component.hpp>
#include <glm/glm.hpp>

namespace our {

    class MovementComponent : public Component 
    {
    public:
        glm::vec3 linearVelocity = {0, 0, 0}; // Each frame, the entity should move as follows: position += linearVelocity * deltaTime 
        glm::vec3 angularVelocity = {0, 0, 0}; // Each frame, the entity should rotate as follows: rotation += angularVelocity * deltaTime

        // The ID of this component type is "Movement"
        static std::string getID() { return "Movement"; }

        virtual std::string getid() { return "Movement";}

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}

