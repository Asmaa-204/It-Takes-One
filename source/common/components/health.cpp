#include <components/health.hpp>
#include <deserialize-utils.hpp>

namespace our {
    // Reads linearVelocity & angularVelocity from the given json object
    void HealthComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        defaultHealth = data.value("maxHealth", defaultHealth);
        currentHealth = defaultHealth;
    }
}