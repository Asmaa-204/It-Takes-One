#pragma once

#include <components/component.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include <string>
#include <glm/glm.hpp>
#include <entities/transform.hpp>

// Types Can Be
///////////////
// Directional Light
// Spot Light
// Poin0 tLight
///////////////
namespace our {

    enum LightType
    {
        Directional = 1,
        Spot = 2,
        Point = 3
    };

    class LightComponent : public Component
    {
    public:
        int light_type;

        // Color
        glm::vec3 color = glm::vec3(1, 1, 1);

        // General Lighting
        glm::vec3 ambient = glm::vec3(1, 1, 1);
        glm::vec3 diffuse = glm::vec3(1, 1, 1);
        glm::vec3 specular = glm::vec3(1, 1, 1);

        // Direction
        glm::vec4 localDirection = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        
        // CutOff
        float innerCutoff = 0.0;
        float outerCutoff = 0.0;

        // Attenuation
        float constantAttenuation = 1.0;
        float linearAttenuation = 0.0;
        float quadraticAttenuation = 0.0;

        // Shiness
        float shininess = 1.0; 

        static std::string getID() { return "Light"; }

        virtual std::string getid() { return "Light";}

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}