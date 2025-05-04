#pragma once

#include <systems/system.hpp>
#include <entities/world.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <components/player.hpp>
#include <components/camera.hpp>
#include <application.hpp>

#include <iostream>

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

                Entity* cameraEntity = world->getEntitiesByTag("Camera").empty() ? nullptr : world->getEntitiesByTag("Camera")[0];
                glm::mat4 cameraTransform = cameraEntity->getLocalToWorldMatrix();

                glm::vec3 cameraForward = glm::normalize(glm::vec3(cameraTransform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
                glm::vec3 cameraForwardFlattened = cameraForward;
                cameraForwardFlattened.y = 0.0f; // Set y component to 0
                cameraForwardFlattened = glm::normalize(cameraForwardFlattened); // Re-normalize
                
                glm::vec3 cameraRight = glm::normalize(glm::vec3(cameraTransform * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
                cameraRight.y = 0.0f; // Set y component to 0
                cameraRight = glm::normalize(cameraRight); // Re-normalize

                glm::vec3 cameraUp = glm::normalize(glm::vec3(cameraTransform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));



                if (player) updateCameraPosition(world, cameraEntity, cameraForward, player);
                if (player && movement) handlePlayerInput(player, cameraForwardFlattened, cameraRight, cameraRight, movement, rigidBody, deltaTime);
            }
        }

    private:
        void handlePlayerInput(PlayerComponent* player, glm::vec3 cameraForward, glm::vec3 cameraRight, glm::vec3 cameraUp, MovementComponent* movement, RigidBodyComponent* rigidBody, float deltaTime) {
            glm::vec3 linearVelocity = glm::vec3(0.0f);
            glm::vec3 angularVelocity = glm::vec3(0.0f);
                        
            if (app->getKeyboard().isPressed(GLFW_KEY_W)) linearVelocity += player->movementSpeed.z * cameraForward;
            if (app->getKeyboard().isPressed(GLFW_KEY_S)) linearVelocity -= player->movementSpeed.z * cameraForward;
            if (app->getKeyboard().isPressed(GLFW_KEY_A)) linearVelocity -= player->movementSpeed.x * cameraRight;
            if (app->getKeyboard().isPressed(GLFW_KEY_D)) linearVelocity += player->movementSpeed.x * cameraRight;
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE)) linearVelocity.y += player->jumpForce;

            movement->linearVelocity = linearVelocity;
            movement->angularVelocity = angularVelocity;
        }

        void updateCameraPosition(World* world, Entity* cameraEntity, glm::vec3 cameraForward, PlayerComponent* player) {
            // std::cout << "calling update position" << std::endl;


            if (!(cameraEntity)) return;

            // Get player's position
            Entity* playerEntity = player->getOwner();
            glm::vec3 playerPosition = playerEntity->localTransform.position;

            glm::vec3 meshCenter = player->getMeshCenter();

            // Get player's transformation matrix
            glm::mat4 playerTransform = playerEntity->getLocalToWorldMatrix();

            // // Extract forward and up vectors
            glm::vec3 playerForward = glm::normalize(glm::vec3(playerTransform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
            
            // Camera offset
            float cameraDistance = 5.0f;
            float cameraHeight = 2.0f;

            // Calculate the camera position based on the mesh center
            glm::vec3 cameraPosition = meshCenter;
            cameraPosition -= cameraForward * cameraDistance; // Move the camera back
            // cameraPosition.z += cameraDistance;
            // cameraPosition.y += cameraHeight;

            // Set the camera position
            cameraEntity->localTransform.position = cameraPosition;

            // Make the camera look at the mesh center
            // glm::vec3 cameraDirection = glm::normalize(meshCenter - cameraPosition);
            // glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f); // Up vector for the camera

            // Set the camera rotation
            // cameraEntity->localTransform.rotation = glm::eulerAngles(glm::quatLookAt(cameraDirection, upVector));
        }
    };
}