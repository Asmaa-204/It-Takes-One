#pragma once

#include <components/component.hpp>
#include <asset-loader.hpp>
#include <string>


namespace our {

    class BulletComponent : public Component
    {
    public:
        static std::string getID() { return "Bullet"; }
        virtual std::string getid() { return "Bullet";}
        
        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}