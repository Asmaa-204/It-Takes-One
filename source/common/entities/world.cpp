#include "world.hpp"
#include "components/rigid-body.hpp"
#include <iostream>

namespace our
{

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json &data, Entity *parent)
    {
        if (!data.is_array())
            return;
        for (const auto &entityData : data)
        {
            Entity *entity = add();
            entity->parent = parent;

            // Deserialize the entity's data
            entity->deserialize(entityData);

            if (entityData.contains("children"))
            {
                deserialize(entityData["children"], entity);
            }

            for (Component *c : entity->components)
            {
                addEntityToTag(c->getid(), entity);
            }
        }
    }

    void World::initializePhysics()
    {
        // create the collision configuration
        collisionConfiguration = new btDefaultCollisionConfiguration();
        // create the dispatcher
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        // create the broadphase
        broadPhase = new btDbvtBroadphase();
        // create the constraint solver
        solver = new btSequentialImpulseConstraintSolver();
        // create the world
        physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfiguration);
    }

    void World::shutdownPhysics()
    {
        delete physicsWorld;
        delete solver;
        delete broadPhase;
        delete dispatcher;
        delete collisionConfiguration;
    }
}