#pragma once

#include <entities/world.hpp>
#include <entities/entity.hpp>
#include <components/rigid-body.hpp>
#include <components/health.hpp>

#include <iostream>

namespace our
{
    class HealthSystem : System
    {
    public:

        void update(World* world, float deltaTime) override {
            checkAllCollisions(world);
        }
    
    private:
        void checkAllCollisions(World* world) {
            int numManifolds = world->getPhysicsWorld()->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++) {
                btPersistentManifold* contactManifold = world->getPhysicsWorld()->getDispatcher()->getManifoldByIndexInternal(i);
                const btCollisionObject* colObj0 = contactManifold->getBody0();
                const btCollisionObject* colObj1 = contactManifold->getBody1();
        
                btRigidBody* body0 = const_cast<btRigidBody*>(btRigidBody::upcast(colObj0));
                btRigidBody* body1 = const_cast<btRigidBody*>(btRigidBody::upcast(colObj1));
        
                body0->activate();
                body1->activate();
        
                if (body0 && body1) {
                    // print the collision information
                    DestroyGameObject(world, world->getEntityByRigidBody(body0), world->getEntityByRigidBody(body1));
                }
            }
        }

        void DestroyGameObject(World* world, Entity* entity1, Entity* entity2) {
            if (!entity1 || !entity2) return;
        
            // get the health components of the entities
            HealthComponent* health1 = entity1->getComponent<HealthComponent>();
            HealthComponent* health2 = entity2->getComponent<HealthComponent>();
            if (health1 && health2) {
                // print the health of the entities
                std::cout << "Entity 1 Health: " << health1->getCurrentHealth() << std::endl;
                std::cout << "Entity 2 Health: " << health2->getCurrentHealth() << std::endl;
                // reduce the health of the entities
                health1->takeDamage(1);
                health2->takeDamage(1);
        
                // check if the entities are alive
                destroyEntity(world, entity1, health1);
                destroyEntity(world, entity2, health2);
            }
        
            
        }

        void destroyEntity(World* world, Entity* entity, HealthComponent* health) {
            if (!health->isAlive()) {
                // destroy entity2
                entity->getWorld()->markForRemoval(entity);
                // delete the rigid body from the pyhsics world
                btRigidBody* body = entity->getComponent<RigidBodyComponent>()->getRigidBody();
                if (body) {
                    world->getPhysicsWorld()->removeRigidBody(body);
                    delete body->getCollisionShape();
                    delete body->getMotionState();
                    delete body;
                }
            }
        }
    };
    
} // namespace our
