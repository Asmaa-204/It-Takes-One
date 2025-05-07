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

namespace our {

    class EnemyAISystem : public System {
        Application *app = nullptr;

        float calculateDistance(const glm::vec3 &pos1, const glm::vec3 &pos2) {
            float dx = pos1.x - pos2.x;
            float dy = pos1.y - pos2.y;
            float dz = pos1.z - pos2.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

    public:
        void enter(Application *app) { this->app = app; }

        void update(World *world, float deltaTime) override {
            Entity *player = world->getEntitiesByTag("Player").front();
            if (!player)
                return;

            std::vector<Entity *> enemies = world->getEntitiesByTag("EnemyAI");
            glm::vec3 *playerPos = &player->localTransform.position;
            for (Entity *enemy : enemies) {
                glm::vec3 *enemyPos = &enemy->localTransform.position;
                EnemyShoot *enemyShoot = enemy->getComponent<EnemyShoot>();
                if (this->calculateDistance(*playerPos, *enemyPos) < 25.0f) {
                    enemyShoot->shoot = true;
                } else {
                    enemyShoot->shoot = false;
                }
            }
        }

    private:
    };
}  // namespace our
