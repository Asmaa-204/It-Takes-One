#pragma once

#include <components/component.hpp>
#include <asset-loader.hpp>
#include <string>
#include <entities/entity.hpp>


namespace our {

    class BulletComponent : public Component
    {
        Entity* shooter = nullptr; // The entity that shot this bullet, if any
    public:
        static std::string getID() { return "Bullet"; }
        virtual std::string getid() { return "Bullet";}

        Entity* getShooter() const { return shooter; }
        // Sets the shooter of this bullet
        void setShooter(Entity* shooter) { this->shooter = shooter; }
        
        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}