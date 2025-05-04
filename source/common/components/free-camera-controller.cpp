#include <components/free-camera-controller.hpp>
#include <entities/entity.hpp>
#include <deserialize-utils.hpp>

namespace our {
    void FreeCameraControllerComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        rotationSensitivity = data.value("rotationSensitivity", rotationSensitivity);
        fovSensitivity = data.value("fovSensitivity", fovSensitivity);
        positionSensitivity = data.value("positionSensitivity", positionSensitivity);
        speedupFactor = data.value("speedupFactor", speedupFactor);
    }
}