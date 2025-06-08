#pragma once

#include <application.hpp>
#include <components/bullet.hpp>
#include <components/camera.hpp>
#include <components/health.hpp>
#include <components/player.hpp>
#include <components/rigid-body.hpp>
#include <entities/entity.hpp>
#include <entities/world.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <systems/sound.hpp>
#include <systems/system.hpp>

namespace our {
    class HealthSystem : public System {
        Application *app = nullptr;
        SoundSystem soundSystem;
        double elapsedTime = 0.0;
        const double damageRate = 1 / 5.0;  // Time in seconds between shots
        
        // Add sound cooldown variables
        float soundCooldown = 0.0f;
        const float SOUND_COOLDOWN_DURATION = 1.0f;  // One second between sounds

    public:
        void enter(Application *app) {
            this->app = app;
            soundSystem = app->getSound();
            soundSystem.loadSound("ouch", "assets/sounds/ouch.wav");
        }

        void update(World *world, float deltaTime) {
            // Update cooldowns
            // update the elapsed time
            elapsedTime += deltaTime;
            soundCooldown = std::max(0.0f, soundCooldown - deltaTime);

            // check if the elapsed time is less than the fire rate
            if (elapsedTime < damageRate)
                return;
            // reset the elapsed time
            elapsedTime = 0.0;

            checkAllCollisions(world);
        }

    private:
        void checkAllCollisions(World *world) {
            int numManifolds =
                world->getPhysicsWorld()->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++) {
                if (i >= world->getPhysicsWorld()
                             ->getDispatcher()
                             ->getNumManifolds()) {
                    continue;  // Skip if the index is no longer valid
                }

                btPersistentManifold *contactManifold =
                    world->getPhysicsWorld()
                        ->getDispatcher()
                        ->getManifoldByIndexInternal(i);
                if (!contactManifold)
                    continue;  // Skip invalid manifolds

                const btCollisionObject *colObj0 = contactManifold->getBody0();
                const btCollisionObject *colObj1 = contactManifold->getBody1();

                btRigidBody *body0 =
                    const_cast<btRigidBody *>(btRigidBody::upcast(colObj0));
                btRigidBody *body1 =
                    const_cast<btRigidBody *>(btRigidBody::upcast(colObj1));

                body0->activate();
                body1->activate();

                if (body0 && body1) {
                    DestroyGameObject(world, world->getEntityByRigidBody(body0),
                                      world->getEntityByRigidBody(body1));
                }
            }
        }

        void DestroyGameObject(World *world, Entity *entity1, Entity *entity2) {
            if (!entity1 || !entity2)
                return;

            // get the health components of the entities
            HealthComponent *health1 = entity1->getComponent<HealthComponent>();
            HealthComponent *health2 = entity2->getComponent<HealthComponent>();

            if (health1 && health2) {
                damagePlayer(world, entity1, entity2);

                damageEntity(entity1, entity2, health1);
                damageEntity(entity2, entity1, health2);

                if (!health1->isAlive()) destroyEntity(world, entity1);
                if (!health2->isAlive()) destroyEntity(world, entity2);
            }
        }

        void damageEntity(Entity *entity1, Entity *entity2, HealthComponent *health1) {
            PlayerComponent *player2 = entity2->getComponent<PlayerComponent>();
            if (player2) return; // enemies don't take damage from hitting players

            BulletComponent *bullet2 = entity2->getComponent<BulletComponent>();
            if (bullet2 && bullet2->getShooter() == entity1) return; // a bullet can't damage its shooter
                
            health1->takeDamage(1); // damage the entity
        }

        void damagePlayer(World *world, Entity *entity1, Entity *entity2) {
            // get the player component of the entity
            PlayerComponent *player = entity1->getComponent<PlayerComponent>();
            if (!player)
                player = entity2->getComponent<PlayerComponent>();

            if (player) {
                // get the camera entity
                Entity *cameraEntity =
                    world->getEntitiesByTag("Camera").front();
                if (!cameraEntity)
                    return;

                // get the camera component
                CameraComponent *cameraComponent =
                    cameraEntity->getComponent<CameraComponent>();
                if (!cameraComponent)
                    return;

                // get the camera's forward vector
                glm::vec3 cameraForward = glm::vec3(
                    cameraComponent->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0.0, 0.0, -1.0f, 0.0));

                // invert the camera's forward vector
                cameraForward = -cameraForward;

                cameraForward.y = 0.75f;
                cameraForward = glm::normalize(cameraForward);

                // get the rigid body component of the player
                RigidBodyComponent *rigidBody =
                    player->getOwner()->getComponent<RigidBodyComponent>();
                if (!rigidBody)
                    return;

                // Play sound only if cooldown is done
                if (soundCooldown <= 0.0f) {
                    soundSystem.playSound("ouch");
                    soundCooldown = SOUND_COOLDOWN_DURATION;
                }

                // set the players speed to the camera's forward vector
                rigidBody->getRigidBody()->setLinearVelocity(
                    btVector3(cameraForward.x, cameraForward.y,
                              cameraForward.z) *
                    5.0f);

                player->setDamaged(true);
            }
        }

        void destroyEntity(World *world, Entity *entity) {
            if (entity->getComponent<PlayerComponent>()) {
                app->restartState();
                return;
            }
            // destroy entity2
            entity->getWorld()->markForRemoval(entity);
            // delete the rigid body from the pyhsics world
            btRigidBody *body =
                entity->getComponent<RigidBodyComponent>()->getRigidBody();
            if (body) {
                world->getPhysicsWorld()->removeRigidBody(body);
                delete body->getCollisionShape();
                delete body->getMotionState();
                delete body;
            }
        }
    };

}  // namespace our
