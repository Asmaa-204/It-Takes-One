#pragma once

#include <entities/world.hpp>
#include <components/player.hpp>
#include <components/rigid-body.hpp>
#include <components/health.hpp>
#include <application.hpp>

#include <iostream>

namespace our
{
    class ShootingSystem : System
    {
        Application* app = nullptr;
        double elapsedTime = 0.0;
        const double fireRate = 1/5.0; // Time in seconds between shots
        const double bulletSpeed = 23.0; // Speed of the bullet in units per second
    public:

        void enter(Application* app) {
            this->app = app;
        }

        void update(World* world, float deltaTime) override {
            // check for mouse input
            if (!(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_LEFT))) return;
            // update the elapsed time
            elapsedTime += deltaTime;

            // check if the elapsed time is less than the fire rate
            if (elapsedTime < fireRate) return;
            // reset the elapsed time
            elapsedTime = 0.0;


            // get the player entity
            Entity* player = world->getEntitiesByTag("Player").front();
            if (!player) return;

            // get the player's player component
            PlayerComponent* playerComponent = player->getComponent<PlayerComponent>();

            // get the center of the player component
            glm::vec3 playerCenter = playerComponent->getMeshCenter();

            // get the camera entity
            Entity* cameraEntity = world->getEntitiesByTag("Camera").front();
            if (!cameraEntity) return;

            // get the camera component
            CameraComponent* cameraComponent = cameraEntity->getComponent<CameraComponent>();
            if (!cameraComponent) return;

            // get the camera's forward vector
            glm::vec3 cameraForward = glm::vec3(cameraComponent->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0));

            // create an entity for the bullet
            Entity* bullet = world->add();

            // set the bullet's position to the player's position
            bullet->localTransform.position = playerCenter + cameraForward * 1.5f;

            // set the bullet's rotation to the camera's rotation
            bullet->localTransform.rotation = cameraComponent->getOwner()->localTransform.rotation;

            // set the scale of the bullet to 0.017
            bullet->localTransform.scale = glm::vec3(0.017f, 0.017f, 0.017f);

            // add mesh component 
            MeshRendererComponent* meshRenderer = bullet->addComponent<MeshRendererComponent>();
            meshRenderer->mesh = AssetLoader<Mesh>::get("bullet");
            meshRenderer->material = AssetLoader<Material>::get("bullet");

            // add a health component
            HealthComponent* health = bullet->addComponent<HealthComponent>();
            health->setDefaultHealth(-1);
            health->resetHealth();

            // create the rigid body component
            RigidBodyComponent* rigidBody = bullet->addComponent<RigidBodyComponent>();
            rigidBody->createRigidBody(.1f);

            // disable gravity for the bullets
            // rigidBody->getRigidBody()->setGravity(btVector3(0, 0, 0));

            cameraForward.y = 0.1f; // set y to 0.0f to avoid the bullet going up
            cameraForward = glm::normalize(cameraForward);

            // set the bullet's linear velocity to the camera's forward vector
            rigidBody->getRigidBody()->setLinearVelocity(btVector3(
                cameraForward.x * bulletSpeed,
                cameraForward.y * bulletSpeed,
                cameraForward.z * bulletSpeed
            ));
        }
    };
    
} // namespace our
