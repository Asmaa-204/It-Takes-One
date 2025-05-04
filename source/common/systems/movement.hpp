#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include <systems/system.hpp>
#include <entities/world.hpp>
#include <components/movement.hpp>
#include <components/rigid-body.hpp>
#include <components/player.hpp>


namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"

    class MovementSystem : System
    {
    public:
        

        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) override {
            // For each entity in the world
            for(auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                PlayerComponent* player = entity->getComponent<PlayerComponent>();
                // If the movement component exists
                if(movement)
                {
                    RigidBodyComponent* rigidBody = entity->getComponent<RigidBodyComponent>();
                    
                    // move the rigid body if the entity has a rigid body componenet
                    if (rigidBody) {
                        rigidBody->getRigidBody()->activate();

                        btVector3 currentLinearVelocity = rigidBody->getRigidBody()->getLinearVelocity();
                        btVector3 updatedLinearVelocity = currentLinearVelocity + btVector3(
                            deltaTime * movement->linearVelocity.x,
                            deltaTime * movement->linearVelocity.y,
                            deltaTime * movement->linearVelocity.z
                        );
                        
                        rigidBody->getRigidBody()->setLinearVelocity(updatedLinearVelocity);
                        
                        //disable rotational movements for player
                        if(!player) {
                            btVector3 currentAngularVelocity = rigidBody->getRigidBody()->getAngularVelocity();
                            btVector3 updatedAngularVelocity = currentAngularVelocity + btVector3(
                                deltaTime * movement->angularVelocity.x,
                                deltaTime * movement->angularVelocity.y,
                                deltaTime * movement->angularVelocity.z
                            );
                            rigidBody->getRigidBody()->setAngularVelocity(updatedAngularVelocity);
                        }

                    } else {
                        // Change the position and rotation based on the linear & angular velocity and delta time.
                        entity->localTransform.position += deltaTime * movement->linearVelocity;
                        entity->localTransform.rotation += deltaTime * movement->angularVelocity;
                    }
                }
            }
        }

    };
}
