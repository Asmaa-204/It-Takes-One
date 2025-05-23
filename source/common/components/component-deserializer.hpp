#pragma once

#include <components/camera.hpp>
#include <components/enemy-ai.hpp>
#include <components/enemy-shoot.hpp>
#include <components/free-camera-controller.hpp>
#include <components/health.hpp>
#include <components/light.hpp>
#include <components/mesh-renderer.hpp>
#include <components/movement.hpp>
#include <components/player.hpp>
#include <components/rigid-body.hpp>
#include <entities/entity.hpp>
#include <iostream>

namespace our {

    // Given a json object, this function picks and creates a component in the
    // given entity based on the "type" specified in the json object which is
    // later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json& data,
                                     Entity* entity) {
        std::string type = data.value("type", "");
        Component* component = nullptr;
        if (type == CameraComponent::getID()) {
            component = entity->addComponent<CameraComponent>();
        } else if (type == FreeCameraControllerComponent::getID()) {
            component = entity->addComponent<FreeCameraControllerComponent>();
        } else if (type == MovementComponent::getID()) {
            component = entity->addComponent<MovementComponent>();
        } else if (type == MeshRendererComponent::getID()) {
            component = entity->addComponent<MeshRendererComponent>();
        } else if (type == LightComponent::getID()) {
            component = entity->addComponent<LightComponent>();
        } else if (type == RigidBodyComponent::getID()) {
            component = entity->addComponent<RigidBodyComponent>();
        } else if (type == PlayerComponent::getID()) {
            component = entity->addComponent<PlayerComponent>();
        } else if (type == HealthComponent::getID()) {
            component = entity->addComponent<HealthComponent>();
        } else if (type == EnemyAI::getID()) {
            component = entity->addComponent<EnemyAI>();
        } else if (type == EnemyShoot::getID()) {
            component = entity->addComponent<EnemyShoot>();
        }

        if (component)
            component->deserialize(data);
    }

}  // namespace our
