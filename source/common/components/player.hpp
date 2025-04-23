#pragma once

#include <components/component.hpp>
#include <glm/glm.hpp>

namespace our {

    class PlayerComponent : public Component {
    public:
        glm::vec3 movementSpeed = glm::vec3(5.0f, 5.0f, 5.0f);
        float jumpForce = 10.0f;

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