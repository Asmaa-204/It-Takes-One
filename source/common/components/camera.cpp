#include <components/camera.hpp>
#include <entities/entity.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace our {
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if(cameraTypeStr == "orthographic"){
            cameraType = CameraType::ORTHOGRAPHIC;
        } else {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();

        glm::vec3 eye = glm::vec3(M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));        
        glm::vec3 center = glm::vec3(M * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));        
        glm::vec3 up = glm::vec3(M * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
        
        return glm::lookAt(eye, center, up);
    }


    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const {

        float aspectRatio = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);

        if(cameraType == CameraType::ORTHOGRAPHIC) {
            float halfHeight = orthoHeight / 2.0f;
            float halfWidth = halfHeight * aspectRatio;
            return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
        }
        else {
            return glm::perspective(fovY, aspectRatio, near, far);
        }
    }
}