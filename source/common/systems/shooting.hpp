#pragma once

#include <application.hpp>
#include <components/bullet.hpp>
#include <components/camera.hpp>
#include <components/enemy-shoot.hpp>
#include <components/health.hpp>
#include <components/player.hpp>
#include <components/rigid-body.hpp>
#include <entities/world.hpp>
#include <iostream>
#include <systems/sound.hpp>
#include <systems/system.hpp>

namespace our {
    class ShootingSystem : System {
        Application* app = nullptr;
        SoundSystem* soundSystem = nullptr;

        double elapsedTime = 0.0;
        const double fireRate = 1 / 5.0;  // Time in seconds between shots
        const double bulletSpeed =
            23.0;  // Speed of the bullet in units per second
        
        const double bulletSize = 0.022f;

        void updatePlayerShooting(World* world, float deltaTime) {
            // get the player entity
            Entity* player = world->getEntitiesByTag("Player").front();
            if (!player)
                return;

            // update the elapsed time
            player->elapsedTime += deltaTime;

            // get the player's player component
            PlayerComponent* playerComponent =
                player->getComponent<PlayerComponent>();

            // check for mouse input
            if (!(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_LEFT) ||
                  app->getKeyboard().isPressed(GLFW_KEY_C)) ||
                player->elapsedTime < fireRate) {
                playerComponent->setShooting(false);
                return;
            }

            // check if the elapsed time is less than the fire rate
            if (player->elapsedTime < fireRate)
                return;

            // reset the elapsed time
            player->elapsedTime = 0.0;

            // play fire sound effect
            if (soundSystem) {
                soundSystem->playSound("fire");
            }

            playerComponent->setShooting(true);

            // get the center of the player component
            glm::vec3 playerCenter = playerComponent->getMeshCenter();
            // glm::vec3 playerCenter = player->localTransform.position;

            // get the camera entity
            Entity* cameraEntity = world->getEntitiesByTag("Camera").front();
            if (!cameraEntity)
                return;

            // get the camera component
            CameraComponent* cameraComponent =
                cameraEntity->getComponent<CameraComponent>();
            if (!cameraComponent)
                return;

            // get the camera's forward vector
            glm::vec3 cameraForward =
                glm::vec3(cameraComponent->getOwner()->getLocalToWorldMatrix() *
                          glm::vec4(0.0, 0.0, -1.0f, 0.0));

            // create an entity for the bullet
            Entity* bullet = world->add();

            // set the bullet's position to the player's position
            bullet->localTransform.position =
                playerCenter +
                glm::vec3(cameraForward.x, 0, cameraForward.z) * 1.5f;

            // set the bullet's rotation to the camera's rotation
            bullet->localTransform.rotation =
                cameraComponent->getOwner()->localTransform.rotation;

            // set the scale of the bullet to 0.017
            bullet->localTransform.scale = glm::vec3(bulletSize, bulletSize, bulletSize);

            this->addBulletComponents(bullet, player);

            // disable gravity for the bullets
            // cameraForward.y = 0.0f; // set x to 0.0f to avoid the bullet
            // going sideways cameraForward = glm::normalize(cameraForward);
            cameraForward.y =
                0.0f;  // set y to 0.0f to avoid the bullet going up
            cameraForward = glm::normalize(cameraForward);

            RigidBodyComponent* rigidBody =
                bullet->getComponent<RigidBodyComponent>();

            // set the bullet's linear velocity to the camera's forward vector
            rigidBody->getRigidBody()->setLinearVelocity(btVector3(
                cameraForward.x * bulletSpeed, cameraForward.y * bulletSpeed,
                cameraForward.z * bulletSpeed));

            rigidBody->getRigidBody()->setGravity(btVector3(0, 0, 0));
        }

        void updateEnemyShooting(World* world, float deltaTime) {
            Entity* player = world->getEntitiesByTag("Player").front();

            std::vector<Entity*> enemies =
                world->getEntitiesByTag("EnemyShoot");
            for (Entity* enemy : enemies) {
                // update the elapsed time
                enemy->elapsedTime += deltaTime;

                EnemyShoot* enemyShoot = enemy->getComponent<EnemyShoot>();
                if (!enemyShoot->shoot || enemy->elapsedTime < fireRate * 5)
                    continue;

                // reset the elapsed time
                enemy->elapsedTime = 0.0;

                // play fire sound effect
                if (soundSystem) {
                    soundSystem->playSound("fire");
                }

                glm::vec3 bulletDirection =
                    glm::normalize(player->localTransform.position -
                                   enemy->localTransform.position);

                // create an entity for the bullet
                Entity* bullet = world->add();

                // set the bullet's position to the player's position
                bullet->localTransform.position =
                    enemy->localTransform.position +
                    glm::vec3(bulletDirection.x, 0.5, bulletDirection.z) * 1.5f;

                // set the scale of the bullet to 0.017
                bullet->localTransform.scale =
                    glm::vec3(0.017f, 0.017f, 0.017f);

                this->addBulletComponents(bullet, enemy);

                RigidBodyComponent* rigidBody =
                    bullet->getComponent<RigidBodyComponent>();

                // set the bullet's linear velocity to the camera's forward
                // vector
                rigidBody->getRigidBody()->setLinearVelocity(
                    btVector3(bulletDirection.x * bulletSpeed,
                              bulletDirection.y * bulletSpeed,
                              bulletDirection.z * bulletSpeed));

                rigidBody->getRigidBody()->setGravity(btVector3(0, 0, 0));
            }
        }

        void addBulletComponents(Entity* bullet, Entity* shooter = nullptr) {
            // add mesh component
            MeshRendererComponent* meshRenderer =
                bullet->addComponent<MeshRendererComponent>();
            meshRenderer->mesh = AssetLoader<Mesh>::get("bullet");
            meshRenderer->material = AssetLoader<Material>::get("bullet");

            // add a health component
            HealthComponent* health = bullet->addComponent<HealthComponent>();
            health->setDefaultHealth(-1);
            health->resetHealth();

            // add a bullet component
            BulletComponent* bulletComponent =
                bullet->addComponent<BulletComponent>();

            // set the shooter of the bullet
            bulletComponent->setShooter(shooter);

            // create the rigid body component
            RigidBodyComponent* rigidBody =
                bullet->addComponent<RigidBodyComponent>();
            rigidBody->createRigidBody(.1f);
        }

    public:
        void enter(Application* app) {
            this->app = app;
            soundSystem = &app->getSound();
            soundSystem->loadSound("fire", "assets/sounds/fire.wav");
        }

        void update(World* world, float deltaTime) override {
            this->updatePlayerShooting(world, deltaTime);
            this->updateEnemyShooting(world, deltaTime);
        }
    };

}  // namespace our
