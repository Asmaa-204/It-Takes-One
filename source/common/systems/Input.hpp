#pragma once

#include <entities/world.hpp>
#include <components/camera.hpp>
#include <components/free-camera-controller.hpp>
#include <application.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our {

    class InputSystem {
        Application* app = nullptr;
        bool mouse_locked = false;

    public:
        void enter(Application* app) {
            this->app = app;
        }

        void update(World* world, float deltaTime) {
            CameraComponent* camera = nullptr;
            FreeCameraControllerComponent* controller = nullptr;

            for(auto entity : world->getEntities()) {
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if(camera && controller) break;
            }

            if(!(camera && controller)) return;
            Entity* entity = camera->getOwner();

            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;

            updateMouseInput(controller, rotation, camera);
            updateKeyboardInput(controller, position, entity, deltaTime);
        }

        void exit() 
        {
            app->getMouse().unlockMouse(app->getWindow());
        }

    private:

        void updateMouseInput(FreeCameraControllerComponent* controller, glm::vec3& rotation, CameraComponent* camera) {
            
            glm::vec2 delta = app->getMouse().getMouseDelta();
            rotation.x -= delta.y * controller->rotationSensitivity;
            rotation.y -= delta.x * controller->rotationSensitivity;
            
            // Clamp pitch
            rotation.x = glm::clamp(rotation.x, -glm::half_pi<float>() * 0.99f, glm::half_pi<float>() * 0.99f);
            // Wrap yaw
            rotation.y = glm::wrapAngle(rotation.y);

            // Update FOV
            float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            camera->fovY = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f);
        }

        void updateKeyboardInput(FreeCameraControllerComponent* controller, glm::vec3& position, Entity* entity, float deltaTime) {

            glm::mat4 matrix = entity->localTransform.toMat4();
            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0));
            glm::vec3 up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0));
            glm::vec3 right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT)) {
                current_sensitivity *= controller->speedupFactor;
            }

            // if(app->getKeyboard().isPressed(GLFW_KEY_W)) position += front * (deltaTime * current_sensitivity.z);
            // if(app->getKeyboard().isPressed(GLFW_KEY_S)) position -= front * (deltaTime * current_sensitivity.z);
            // if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position += up * (deltaTime * current_sensitivity.y);
            // if(app->getKeyboard().isPressed(GLFW_KEY_E)) position -= up * (deltaTime * current_sensitivity.y);
            // if(app->getKeyboard().isPressed(GLFW_KEY_D)) position += right * (deltaTime * current_sensitivity.x);
            // if(app->getKeyboard().isPressed(GLFW_KEY_A)) position -= right * (deltaTime * current_sensitivity.x);
            // if(app->getKeyboard().isPressed(GLFW_KEY_TAB)) app->getMouse().toggleMouse(app->getWindow());
        }
    };

}
