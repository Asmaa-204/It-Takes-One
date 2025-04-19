#pragma once

#include <entities/world.hpp>
#include <components/rigid-body.hpp>

#include <iostream>

namespace our
{
    class PhysicsSystem : System
    {
    public:

        void update(World* world, float deltaTime) override {
            // Run the physics world
            world->getPhysicsWorld()->stepSimulation(deltaTime);
            int i = 0;
            // For each entity in the world get the updated rigid body transform and update the entity local transform
            for (auto entity: world->getEntities()) {
                RigidBodyComponent* rigidBody = entity->getComponent<RigidBodyComponent>();

                if (rigidBody) {
                    btTransform worldTransform;

                    rigidBody->getRigidBody()->getMotionState()->getWorldTransform(worldTransform);

                    entity->localTransform.position.x = worldTransform.getOrigin().getX();
                    entity->localTransform.position.y = worldTransform.getOrigin().getY();
                    entity->localTransform.position.z = worldTransform.getOrigin().getZ();

                    entity->localTransform.rotation.x = worldTransform.getRotation().getX();
                    entity->localTransform.rotation.y = worldTransform.getRotation().getY();
                    entity->localTransform.rotation.z = worldTransform.getRotation().getZ();
                }
            }

        }
    };
    
} // namespace our
