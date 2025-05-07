#pragma once

#include <entities/entity.hpp>
#include <components/component.hpp>
#include <components/mesh-renderer.hpp>
#include <glm/glm.hpp>

#include <iostream>

namespace our {

    class PlayerComponent : public Component {
        bool isShooting;
        bool isDamaged;
    public:
        glm::vec3 movementSpeed = glm::vec3(5.0f, 5.0f, 5.0f);
        float jumpForce = 10.0f;

        //retrieve the mesh center
        glm::vec3 getMeshCenter() const {
            auto owner = getOwner();
            if (!owner) return glm::vec3(0.0f);

            //retrieve the MeshRendererComponent
            MeshRendererComponent* meshRenderer = owner->getComponent<MeshRendererComponent>();
            if (!meshRenderer || !meshRenderer->mesh) return glm::vec3(0.0f);

            //get the local center of the mesh
            glm::vec3 localCenter = meshRenderer->mesh->getCenter();

            //transform the local center to world space using the entity's transform
            glm::mat4 localToWorld = owner->getLocalToWorldMatrix();
            glm::vec3 worldCenter = glm::vec3(localToWorld * glm::vec4(localCenter, 1.0f));

            return worldCenter;
        }

        void setShooting(bool shooting) {
            isShooting = shooting;
        }
        bool getShooting() const {
            return isShooting;
        }
        void setDamaged(bool damaged) {
            isDamaged = damaged;
        }
        bool getDamaged() const {
            return isDamaged;
        }


        // Deserialize the component from JSON
        void deserialize(const nlohmann::json& data) override {
            if (data.contains("movementSpeed")) {
                if (data["movementSpeed"].is_array() && data["movementSpeed"].size() == 3) {
                    movementSpeed = glm::vec3(
                        data["movementSpeed"][0].get<float>(),
                        data["movementSpeed"][1].get<float>(),
                        data["movementSpeed"][2].get<float>()
                    );
                }
            }
            if (data.contains("jumpForce")) {
                jumpForce = data["jumpForce"].get<float>();
            }
        }

        static std::string getID() { return "Player"; }
        virtual std::string getid() { return "Player"; }
    };

}