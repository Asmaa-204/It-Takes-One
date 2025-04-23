#pragma once

#include <systems/system.hpp>
#include <entities/world.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <components/player.hpp>
#include <application.hpp>

namespace our {

    class PlayerSystem : public System {
        Application* app = nullptr;

    public:
        void enter(Application* app) {
            this->app = app;
        }
        void update(World* world, float deltaTime) override {
            for (auto entity : world->getEntities()) {
                PlayerComponent* player = entity->getComponent<PlayerComponent>();
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                RigidBodyComponent* rigidBody = entity->getComponent<RigidBodyComponent>();

                
                if (player && movement) handlePlayerInput(player, movement, rigidBody, deltaTime);
            }
        }

    private:
        void handlePlayerInput(PlayerComponent* player, MovementComponent* movement, RigidBodyComponent* rigidBody, float deltaTime) {
            glm::vec3 linearVelocity = glm::vec3(0.0f);
            glm::vec3 angularVelocity = glm::vec3(0.0f);
                        
            if (app->getKeyboard().isPressed(GLFW_KEY_W)) linearVelocity.z += player->movementSpeed.z;
            if (app->getKeyboard().isPressed(GLFW_KEY_S)) linearVelocity.z -= player->movementSpeed.z;
            if (app->getKeyboard().isPressed(GLFW_KEY_A)) linearVelocity.x -= player->movementSpeed.x;
            if (app->getKeyboard().isPressed(GLFW_KEY_D)) linearVelocity.x += player->movementSpeed.x;
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE)) linearVelocity.y += player->jumpForce;

            movement->linearVelocity = linearVelocity;
            movement->angularVelocity = angularVelocity;
        }
    };

}