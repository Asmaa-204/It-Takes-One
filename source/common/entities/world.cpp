#include "world.hpp"

#include <iostream>

namespace our {

    // This will deserialize a json array of entities and add the new entities
    // to the current world If parent pointer is not null, the new entities will
    // be have their parent set to that given pointer If any of the entities has
    // children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json &data, Entity *parent)
    {
        if (!data.is_array())
            return;
        for (const auto &entityData : data) {
            Entity *entity = add();
            entity->parent = parent;

            // Deserialize the entity's data
            entity->deserialize(entityData);

            if (entityData.contains("children")) {
                deserialize(entityData["children"], entity);
            }

            for (Component *c : entity->components) {
                addEntityToTag(c->getid(), entity);
            }
        }
    }

    void World::initializePhysics() {
        // create the collision configuration
        collisionConfiguration = new btDefaultCollisionConfiguration();
        // create the dispatcher
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        // create the broadphase
        broadPhase = new btDbvtBroadphase();
        // create the constraint solver
        solver = new btSequentialImpulseConstraintSolver();
        // create the world
        physicsWorld = new btDiscreteDynamicsWorld(
            dispatcher, broadPhase, solver, collisionConfiguration);
    }

    void World::shutdownPhysics() {
        delete physicsWorld;
        delete solver;
        delete broadPhase;
        delete dispatcher;
        delete collisionConfiguration;
    }


    Entity *World::createEnemy(glm::vec3 position)
    {
        Entity *enemy = this->add();

        // Set the transformation of the entity
        enemy->localTransform.position = position;
        enemy->localTransform.scale = this->enemyScale;

        // Create the mesh render component
        MeshRendererComponent *meshComponent =
            enemy->addComponent<MeshRendererComponent>();
        meshComponent->mesh = AssetLoader<Mesh>::get("enemy-turtle");
        meshComponent->material = AssetLoader<Material>::get("enemy-turtle");
        addEntityToTag(meshComponent->getid(), enemy);

        // Create a rigid body component
        RigidBodyComponent *rigidComponent =
            enemy->addComponent<RigidBodyComponent>();
        rigidComponent->createRigidBody(1.0f);
        addEntityToTag(rigidComponent->getid(), enemy);

        // Create a health component
        HealthComponent *healthComponent =
            enemy->addComponent<HealthComponent>();
        healthComponent->setDefaultHealth(5);
        healthComponent->setCurrentHealth(5);
        addEntityToTag(healthComponent->getid(), enemy);

        return enemy;
    }
}  // namespace our
