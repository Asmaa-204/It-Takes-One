#pragma once

#include <components/component.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include <string>
#include <glm/glm.hpp>
#include <entities/transform.hpp>
#include <btBulletDynamicsCommon.h>


namespace our {

    class HealthComponent : public Component
    {
        int defaultHealth = 5; // Default health value
        int currentHealth; // Current health value
    public:
        
        
        static std::string getID() { return "Health"; }
        virtual std::string getid() { return "Health";}
        
        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;

        void resetHealth() { currentHealth = defaultHealth; } // Resets the health to the default value
        void takeDamage(int damage) { currentHealth -= damage; } // Reduces the current health by the damage value
        bool isAlive() const { return currentHealth > 0; } // Checks if the entity is alive
        int getCurrentHealth() const { return currentHealth; } // Returns the current health value
        int getDefaultHealth() const { return defaultHealth; } // Returns the default health value
        void setDefaultHealth(int health) { defaultHealth = health; } // Sets the default health value
    };
}