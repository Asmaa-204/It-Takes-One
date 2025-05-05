#include <components/bullet.hpp>
#include <deserialize-utils.hpp>

namespace our {
    // Reads linearVelocity & angularVelocity from the given json object
    void BulletComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
    }
}