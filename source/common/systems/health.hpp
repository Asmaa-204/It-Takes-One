#pragma once

#include <systems/system.hpp>
#include <systems/sound.hpp>

#include <entities/world.hpp>
#include <entities/entity.hpp>

#include <components/rigid-body.hpp>
#include <components/health.hpp>
#include <components/player.hpp>
#include <components/camera.hpp>

#include <glm/glm.hpp>
#include <application.hpp>

#include <iostream>

namespace our
{
    class HealthSystem : public System
    {
        Application *app = nullptr;
        SoundSystem soundSystem;
        double elapsedTime = 0.0;
        const double damageRate = 1/5.0; // Time in seconds between shots

    public:
        void enter(Application *app)
        {
            this->app = app;
            soundSystem = app->getSound();
            soundSystem.loadSound("ouch", "assets/sounds/ouch.wav");
        }

        void update(World *world, float deltaTime)
        {
            // update the elapsed time
            elapsedTime += deltaTime;

            // check if the elapsed time is less than the fire rate
            if (elapsedTime < damageRate) return;
            // reset the elapsed time
            elapsedTime = 0.0;

            checkAllCollisions(world);
        }

    private:
        void checkAllCollisions(World *world)
        {
            int numManifolds = world->getPhysicsWorld()->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++)
            {
                btPersistentManifold *contactManifold = world->getPhysicsWorld()->getDispatcher()->getManifoldByIndexInternal(i);
                const btCollisionObject *colObj0 = contactManifold->getBody0();
                const btCollisionObject *colObj1 = contactManifold->getBody1();

                btRigidBody *body0 = const_cast<btRigidBody *>(btRigidBody::upcast(colObj0));
                btRigidBody *body1 = const_cast<btRigidBody *>(btRigidBody::upcast(colObj1));

                body0->activate();
                body1->activate();

                if (body0 && body1)
                {
                    // print the collision information
                    DestroyGameObject(world, world->getEntityByRigidBody(body0), world->getEntityByRigidBody(body1));
                }
            }
        }

        void DestroyGameObject(World *world, Entity *entity1, Entity *entity2)
        {
            if (!entity1 || !entity2)
                return;

            // get the health components of the entities
            HealthComponent *health1 = entity1->getComponent<HealthComponent>();
            HealthComponent *health2 = entity2->getComponent<HealthComponent>();
            if (health1 && health2)
            {
                // print the health of the entities
                std::cout << "Entity 1 Health: " << health1->getCurrentHealth() << std::endl;
                std::cout << "Entity 2 Health: " << health2->getCurrentHealth() << std::endl;
                // reduce the health of the entities
                health1->takeDamage(1);
                health2->takeDamage(1);

                damagePlayer(world, entity1, entity2);

                // check if the entities are alive
                destroyEntity(world, entity1, health1);
                destroyEntity(world, entity2, health2);
            }
        }

        void damagePlayer(World * world, Entity *entity1, Entity *entity2)
        {
            // get the player component of the entity
            PlayerComponent *player = entity1->getComponent<PlayerComponent>();
            if (!player)
                player = entity2->getComponent<PlayerComponent>();

            if (player)
            {
                // get the camera entity
                Entity *cameraEntity = world->getEntitiesByTag("Camera").front();
                if (!cameraEntity)
                    return;

                // get the camera component
                CameraComponent *cameraComponent = cameraEntity->getComponent<CameraComponent>();
                if (!cameraComponent)
                    return;

                // get the camera's forward vector
                glm::vec3 cameraForward = glm::vec3(cameraComponent->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0));

                // invert the camera's forward vector
                cameraForward = -cameraForward;

                cameraForward.y = 0.75f;
                cameraForward = glm::normalize(cameraForward);

                // get the rigid body component of the player
                RigidBodyComponent *rigidBody = player->getOwner()->getComponent<RigidBodyComponent>();
                if (!rigidBody)
                    return;

                soundSystem.playSound("ouch");

                // set the players speed to the camera's forward vector
                rigidBody->getRigidBody()->setLinearVelocity(btVector3(cameraForward.x, cameraForward.y, cameraForward.z) * 7.0f);
                std::cout << "RIGID BODY MOVED\n";
            }
        }

        void destroyEntity(World *world, Entity *entity, HealthComponent *health)
        {
            if (!health->isAlive())
            {
                // destroy entity2
                entity->getWorld()->markForRemoval(entity);
                // delete the rigid body from the pyhsics world
                btRigidBody *body = entity->getComponent<RigidBodyComponent>()->getRigidBody();
                if (body)
                {
                    world->getPhysicsWorld()->removeRigidBody(body);
                    delete body->getCollisionShape();
                    delete body->getMotionState();
                    delete body;
                }
            }
        }
    };

}
