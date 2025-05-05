#pragma once

#include <systems/system.hpp>
#include <entities/world.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <components/player.hpp>
#include <components/camera.hpp>
#include <application.hpp>
#include <systems/sound.hpp>

#include <iostream>

namespace our
{

    class PlayerSystem : public System
    {
        Application *app = nullptr;

    public:
        void enter(Application *app)
        {
            this->app = app;
            this->soundSystem = app->getSound();
            initializeSounds();
        }

        void update(World *world, float deltaTime) override
        {
            // get the player entity
            Entity *playerEntity = world->getEntitiesByTag("Player").empty() ? nullptr : world->getEntitiesByTag("Player")[0];
            PlayerComponent *playerComponent = playerEntity->getComponent<PlayerComponent>();
            MovementComponent *movement = playerEntity->getComponent<MovementComponent>();
            RigidBodyComponent *rigidBody = playerEntity->getComponent<RigidBodyComponent>();
            
            Entity *cameraEntity = world->getEntitiesByTag("Camera").empty() ? nullptr : world->getEntitiesByTag("Camera")[0];
            glm::mat4 cameraTransform = cameraEntity->getLocalToWorldMatrix();

            glm::vec3 cameraForward = glm::normalize(glm::vec3(cameraTransform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
            glm::vec3 cameraForwardFlattened = cameraForward;
            cameraForwardFlattened.y = 0.0f;                                 // Set y component to 0
            cameraForwardFlattened = glm::normalize(cameraForwardFlattened); // Re-normalize

            glm::vec3 cameraRight = glm::normalize(glm::vec3(cameraTransform * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
            cameraRight.y = 0.0f;                      // Set y component to 0
            cameraRight = glm::normalize(cameraRight); // Re-normalize

            if (playerComponent)
                updateCameraPosition(world, cameraEntity, cameraForward, playerComponent);
            if (playerComponent && movement)
                handlePlayerInput(playerComponent, cameraForwardFlattened, cameraRight, movement, rigidBody, deltaTime);
            if (playerComponent && rigidBody)
            {
                orientCharater(playerEntity, rigidBody, cameraEntity->localTransform.rotation.y);
                checkPlayerFallDeath(playerEntity);
            }
        }

    private:
        SoundSystem soundSystem;
        float jumpSoundCooldown = 0.0f;
        const float JUMP_SOUND_DURATION = 0.5f;
        const float DEATH_HEIGHT_THRESHOLD = -5.0f;
        const float MAX_JUMP_HEIGHT = 1.0f;

        void initializeSounds()
        {
            soundSystem.loadSound("jump", "assets/sounds/jump.wav");
        }

        void handlePlayerInput(PlayerComponent *player, glm::vec3 cameraForward, glm::vec3 cameraRight, MovementComponent *movement, RigidBodyComponent *rigidBody, float deltaTime)
        {
            // Update cooldown timer
            if (jumpSoundCooldown > 0.0f)
            {
                jumpSoundCooldown -= deltaTime;
            }

            glm::vec3 linearVelocity = glm::vec3(0.0f);
            glm::vec3 angularVelocity = glm::vec3(0.0f);

            float heightAboveGround = getHeightAboveGround(player->getOwner());

            if (app->getKeyboard().isPressed(GLFW_KEY_W))
                linearVelocity += player->movementSpeed.z * cameraForward;
            if (app->getKeyboard().isPressed(GLFW_KEY_S))
                linearVelocity -= player->movementSpeed.z * cameraForward;
            if (app->getKeyboard().isPressed(GLFW_KEY_A))
                linearVelocity -= player->movementSpeed.x * cameraRight;
            if (app->getKeyboard().isPressed(GLFW_KEY_D))
                linearVelocity += player->movementSpeed.x * cameraRight;
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE) && heightAboveGround < MAX_JUMP_HEIGHT)
            {
                linearVelocity.y += player->jumpForce;

                // Only play sound if cooldown has expired
                if (jumpSoundCooldown <= 0.0f)
                {
                    soundSystem.playSound("jump");
                    jumpSoundCooldown = JUMP_SOUND_DURATION;
                }
            }

            movement->linearVelocity = linearVelocity;
            movement->angularVelocity = angularVelocity;
        }

        void updateCameraPosition(World *world, Entity *cameraEntity, glm::vec3 cameraForward, PlayerComponent *player)
        {
            if (!(cameraEntity))
                return;

            // Get player's position
            Entity *playerEntity = player->getOwner();
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

            // Set the camera position
            cameraEntity->localTransform.position = cameraPosition;
        }

        void orientCharater(Entity *playerEntity, RigidBodyComponent *rigidBody, float y)
        {
            if (!rigidBody)
                return;

            // // set the entity's local transformation
            
            btTransform worldTransform;

            rigidBody->getRigidBody()->getMotionState()->getWorldTransform(worldTransform);

            // Get the current rotation
            btQuaternion rotation = worldTransform.getRotation();

            // Reset X and Z components of the rotation and orient the character to face the camera
            rotation.setX(0);
            rotation.setZ(0);
            rotation.setY(y + M_PI);
            rotation.normalize();
            
            // Apply the modified rotation back to the rigid body
            worldTransform.setRotation(rotation);
            rigidBody->getRigidBody()->getMotionState()->setWorldTransform(worldTransform);
            
            playerEntity->localTransform.rotation.x = worldTransform.getRotation().getX();
            playerEntity->localTransform.rotation.y = y + M_PI;
            // playerEntity->localTransform.rotation.y = worldTransform.getRotation().getY();
            playerEntity->localTransform.rotation.z = worldTransform.getRotation().getZ();
        }

        void checkPlayerFallDeath(Entity *playerEntity)
        {
            if (!playerEntity)
                return;

            float playerY = playerEntity->localTransform.position.y;

            if (playerY < DEATH_HEIGHT_THRESHOLD)
            {
                std::cout << "Player died from falling!" << std::endl;
                app->changeState("play");
            }
        }

        float getHeightAboveGround(Entity *playerEntity)
        {
            if (!playerEntity)
                return 0.0f;
            World *world = playerEntity->getWorld();

            auto *rigidBody = playerEntity->getComponent<RigidBodyComponent>();
            if (!rigidBody)
                return 0.0f;

            const float RAY_LENGTH = 100.0f;

            btVector3 start(
                playerEntity->localTransform.position.x,
                playerEntity->localTransform.position.y,
                playerEntity->localTransform.position.z);

            btVector3 end = start;
            end.setY(start.y() - RAY_LENGTH);

            btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
            world->getPhysicsWorld()->rayTest(start, end, rayCallback);

            if (rayCallback.hasHit())
            {
                float groundY = rayCallback.m_hitPointWorld.y();
                float playerY = playerEntity->localTransform.position.y;
                return playerY - groundY;
            }

            return RAY_LENGTH;
        }
    };
}