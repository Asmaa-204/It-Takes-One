#pragma once

#include <application.hpp>
#include <components/camera.hpp>
#include <components/enemy-ai.hpp>
#include <components/enemy-shoot.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <entities/world.hpp>
#include <systems/sound.hpp>
#include <systems/system.hpp>

namespace our
{

    class EnemyAISystem : public System
    {
        Application *app = nullptr;

        float calculateDistance(const glm::vec3 &pos1, const glm::vec3 &pos2)
        {
            float dx = pos1.x - pos2.x;
            float dy = pos1.y - pos2.y;
            float dz = pos1.z - pos2.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

    public:
        void enter(Application *app) { this->app = app; }

        void update(World *world, float deltaTime) override
        {
            Entity *player = world->getEntitiesByTag("Player").front();
            if (!player)
                return;

            glm::vec3 *playerPos = &player->localTransform.position;
            std::vector<Entity *> enemies = world->getEntitiesByTag("EnemyAI");

            Entity *cameraEntity = world->getEntitiesByTag("Camera").empty() ? nullptr : world->getEntitiesByTag("Camera")[0];
            glm::mat4 cameraTransform = cameraEntity->getLocalToWorldMatrix();

            Entity *playerEntity = world->getEntitiesByTag("Player").empty() ? nullptr : world->getEntitiesByTag("Player")[0];
            glm::vec3 playerPosition = playerEntity->localTransform.position;

            // Loop over all enemies, and check if they are within range of
            // player
            for (Entity *enemy : enemies)
            {
                glm::vec3 *enemyPos = &enemy->localTransform.position;
                EnemyShoot *enemyShoot = enemy->getComponent<EnemyShoot>();
                float distance = this->calculateDistance(*playerPos, *enemyPos);

                if (distance < 7.0f)
                {
                    enemyShoot->shoot = true;
                }
                else
                {
                    enemyShoot->shoot = false;
                }

                // orient the enemy to look into the player
                glm::mat4 enemyTransform = enemy->getLocalToWorldMatrix();
                glm::vec3 enemyForward = glm::normalize(glm::vec3(enemyTransform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
                glm::vec3 enemyForwardFlattened = enemyForward;
                enemyForwardFlattened.y = 0.0f;                                // Set y component to 0
                enemyForwardFlattened = glm::normalize(enemyForwardFlattened); // Re-normalize

                glm::vec3 enemyPosition = enemy->localTransform.position;
                glm::vec3 enemyToPlayer = playerPosition - enemyPosition;
                enemyToPlayer.y = 0;
                enemyToPlayer = glm::normalize(enemyToPlayer);

                // Calculate the angle using dot and cross products
                float dotProduct = glm::dot(enemyForwardFlattened, enemyToPlayer);
                float crossProduct = enemyForwardFlattened.x * enemyToPlayer.z - enemyForwardFlattened.z * enemyToPlayer.x;

                float yRotationAngle = atan2(crossProduct, dotProduct);

                // get the rigid body component of the enemy
                RigidBodyComponent *rigidBody = enemy->getComponent<RigidBodyComponent>();
                if (!rigidBody)
                    return;

                btTransform worldTransform;

                rigidBody->getRigidBody()->getMotionState()->getWorldTransform(worldTransform);

                // Get the current rotation
                btQuaternion rotation = worldTransform.getRotation();

                enemy->localTransform.rotation.y -= yRotationAngle + M_PI;

                // Reset X and Z components of the rotation and orient the character to face the camera
                rotation.setX(0);
                rotation.setZ(0);
                rotation.setY(enemy->localTransform.rotation.y);
                rotation.normalize();

                // Apply the modified rotation back to the rigid body
                worldTransform.setRotation(rotation);
                rigidBody->getRigidBody()->getMotionState()->setWorldTransform(worldTransform);

                enemy->localTransform.rotation.x = worldTransform.getRotation().getX();
                enemy->localTransform.rotation.z = worldTransform.getRotation().getZ();
            }
        }

    private:
    };
} // namespace our
