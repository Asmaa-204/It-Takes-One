#pragma once

#include <unordered_set>
#include <unordered_map>
#include "entity.hpp"

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

namespace our {

    // This class holds a set of entities
    class World {
        std::unordered_set<Entity*> entities; // These are the entities held by this world
        std::unordered_set<Entity*> markedForRemoval; // These are the entities that are awaiting to be deleted when deleteMarkedEntities is called
        std::unordered_map<std::string, std::vector<Entity*>> entitiesByTag;

        // The physics world responsible for all game physics and its componenets
        btDynamicsWorld* physicsWorld;
        btConstraintSolver* solver;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* broadPhase;
        btCollisionConfiguration* collisionConfiguration;
    public:

        World() = default;

        // This will deserialize a json array of entities and add the new entities to the current world
        // If parent pointer is not null, the new entities will be have their parent set to that given pointer
        // If any of the entities has children, this function will be called recursively for these children
        void deserialize(const nlohmann::json& data, Entity* parent = nullptr);

        // This adds an entity to the entities set and returns a pointer to that entity
        // WARNING The entity is owned by this world so don't use "delete" to delete it, instead, call "markForRemoval"
        // to put it in the "markedForRemoval" set. The elements in the "markedForRemoval" set will be removed and
        // deleted when "deleteMarkedEntities" is called.
        Entity* add() {
            Entity *entity = new Entity();
            entity->world = this;
            entities.insert(entity);
            return entity;
        }

        // This returns and immutable reference to the set of all entites in the world.
        const std::unordered_set<Entity*>& getEntities() {
            return entities;
        }

        const std::vector<Entity*>& getEntitiesByTag(const std::string& tag) 
        {
            return entitiesByTag[tag];
        }

        void addEntitiyToTag(const std::string& tag, Entity* entity)
        {
            if(!entity || tag == "")
                return;

            if(entitiesByTag.count(tag))
                entitiesByTag[tag].push_back(entity);
            else
                entitiesByTag[tag] = {entity};
        }

        void initializePhysics();
        void shutdownPhysics();

        // This marks an entity for removal by adding it to the "markedForRemoval" set.
        // The elements in the "markedForRemoval" set will be removed and deleted when "deleteMarkedEntities" is called.
        void markForRemoval(Entity* entity){
           if(entities.find(entity) != entities.end()) {
                markedForRemoval.insert(entity);
            }
        }


        void deleteMarkedEntities(){
            for(auto entity : markedForRemoval) {
                entities.erase(entity);
                delete entity;
            }
            markedForRemoval.clear();
        }

        void clear(){
            for(auto entity : entities) {
                delete entity;
            }
            entities.clear();
            markedForRemoval.clear();
        }

        ~World(){
            clear();
            shutdownPhysics();
        }

        // The world should not be copyable
        World(const World&) = delete;
        World &operator=(World const &) = delete;
    };

}