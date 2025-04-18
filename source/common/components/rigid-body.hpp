#pragma once

#include <components/component.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include <string>
#include <glm/glm.hpp>
#include <entities/transform.hpp>
#include <btBulletDynamicsCommon.h>


namespace our {

    class RigidBodyComponent : public Component
    {
        btRigidBody* rigidBody;

        btCollisionShape* deepCopyCollisionShape(const btCollisionShape* original);
    public:
        

        static std::string getID() { return "Rigid Body"; }

        virtual std::string getid() { return "Rigid Body";}

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
        inline btRigidBody* getRigidBody() { return rigidBody; }
    };
}