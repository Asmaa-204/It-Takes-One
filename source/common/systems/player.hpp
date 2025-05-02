#pragma once

#include <systems/system.hpp>
#include <entities/world.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <components/player.hpp>
#include <components/camera.hpp>
#include <application.hpp>
#include <systems/sound.hpp>

namespace our {

    class PlayerSystem : public System {
        Application* app = nullptr;

    public:
        void enter(Application* app) {
            this->app = app;
            this->soundSystem = app->getSound();
            initializeSounds();
        }
        void update(World* world, float deltaTime) override {
            for (auto entity : world->getEntities()) {
                PlayerComponent* player = entity->getComponent<PlayerComponent>();
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                RigidBodyComponent* rigidBody = entity->getComponent<RigidBodyComponent>();

                if (player) updateCameraPosition(world, player);
                if (player && movement) handlePlayerInput(player, movement, rigidBody, deltaTime);
            }
        }

    private:
        SoundSystem soundSystem;
        float jumpSoundCooldown = 0.0f;
        const float JUMP_SOUND_DURATION = 0.5f; // Adjust this value based on your jump sound length

        void initializeSounds() {
            // Load your sound files
            soundSystem.loadSound("jump", "/home/asmaa/Desktop/It-Takes-One/assets/sounds/jump.wav");
            // soundSystem.loadSound("walk", "assets/sounds/walk.wav");
        }

        void handlePlayerInput(PlayerComponent* player, MovementComponent* movement, RigidBodyComponent* rigidBody, float deltaTime) {
            // Update cooldown timer
            if (jumpSoundCooldown > 0.0f) {
                jumpSoundCooldown -= deltaTime;
            }

            glm::vec3 linearVelocity = glm::vec3(0.0f);
            glm::vec3 angularVelocity = glm::vec3(0.0f);
                        
            if (app->getKeyboard().isPressed(GLFW_KEY_W)) linearVelocity.z += player->movementSpeed.z;
            if (app->getKeyboard().isPressed(GLFW_KEY_S)) linearVelocity.z -= player->movementSpeed.z;
            if (app->getKeyboard().isPressed(GLFW_KEY_A)) linearVelocity.x -= player->movementSpeed.x;
            if (app->getKeyboard().isPressed(GLFW_KEY_D)) linearVelocity.x += player->movementSpeed.x;
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE)) {
                linearVelocity.y += player->jumpForce;
                
                // Only play sound if cooldown has expired
                if (jumpSoundCooldown <= 0.0f) {
                    soundSystem.playSound("jump");
                    jumpSoundCooldown = JUMP_SOUND_DURATION;
                }
            }

            movement->linearVelocity = linearVelocity;
            movement->angularVelocity = angularVelocity;
        }

        void updateCameraPosition(World* world, PlayerComponent* player) {
            // Find the camera entity
            CameraComponent* camera = nullptr;
            Entity* cameraEntity = nullptr;

            for (auto entity : world->getEntities()) {
                camera = entity->getComponent<CameraComponent>();
                if (camera) {
                    cameraEntity = entity;
                    break;
                }
            }

            if (!camera || !cameraEntity) return;

            // Get player's position
            Entity *playerEntity = player->getOwner();

            // Get player's transformation matrix
            glm::mat4 playerTransform = playerEntity->getLocalToWorldMatrix();

            glm::vec3 playerPosition = playerEntity->localTransform.position;

            // // Extract forward and up vectors
            glm::vec3 playerForward = glm::normalize(glm::vec3(playerTransform * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
            
            // Camera offset
            float cameraDistance = 2.0f;
            float cameraHeight = 2.0f;

            // Calculate the camera position based on the mesh center
            glm::vec3 cameraPosition = playerPosition - playerForward;

            cameraPosition.z -= cameraDistance;
            cameraPosition.y += cameraHeight;


            // Set the camera position
            cameraEntity->localTransform.position = cameraPosition;

            
            glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
            // Set the camera rotation
            cameraEntity->localTransform.rotation = glm::eulerAngles(glm::quatLookAt(playerForward, upVector));
            glm::vec4 cameraLookAt = camera->getViewMatrix() * glm::vec4(0, 0, -1, 0);
        }
    };
}