#include "mouse.hpp"
#include <cstring>

namespace our {

    void Mouse::enable(GLFWwindow* window) {
        enabled = true;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        previousMousePosition = currentMousePosition = glm::vec2((float)x, (float)y);
        for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; button++) {
            currentMouseButtons[button] = previousMouseButtons[button] = glfwGetMouseButton(window, button);
        }
        scrollOffset = glm::vec2();
    }

    void Mouse::disable() {
        enabled = false;
        for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; button++) {
            currentMouseButtons[button] = previousMouseButtons[button] = false;
        }
        scrollOffset = glm::vec2();
    }

    void Mouse::update() {
        if (!enabled) return;
        previousMousePosition = currentMousePosition;
        std::memcpy(previousMouseButtons, currentMouseButtons, sizeof(previousMouseButtons));
        scrollOffset = glm::vec2();
    }

    const glm::vec2& Mouse::getMousePosition() const {
        return currentMousePosition;
    }

    glm::vec2 Mouse::getMouseDelta() const {
        return currentMousePosition - previousMousePosition;
    }

    bool Mouse::isPressed(int button) const {
        return currentMouseButtons[button];
    }

    bool Mouse::justPressed(int button) const {
        return currentMouseButtons[button] && !previousMouseButtons[button];
    }

    bool Mouse::justReleased(int button) const {
        return !currentMouseButtons[button] && previousMouseButtons[button];
    }

    const glm::vec2& Mouse::getScrollOffset() const {
        return scrollOffset;
    }

    void Mouse::CursorMoveEvent(double x_position, double y_position) {
        if (!enabled) return;
        currentMousePosition = glm::vec2((float)x_position, (float)y_position);
    }

    void Mouse::MouseButtonEvent(int button, int action, int) {
        if (!enabled) return;
        if (action == GLFW_PRESS) currentMouseButtons[button] = true;
        else if (action == GLFW_RELEASE) currentMouseButtons[button] = false;
    }

    void Mouse::ScrollEvent(double x_offset, double y_offset) {
        if (!enabled) return;
        scrollOffset += glm::vec2((float)x_offset, (float)y_offset);
    }

    void Mouse::lockMouse(GLFWwindow* window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Mouse::unlockMouse(GLFWwindow* window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    bool Mouse::isEnabled() const {
        return enabled;
    }

    void Mouse::setEnabled(bool enabled, GLFWwindow* window) {
        if (this->enabled != enabled) {
            if (enabled) enable(window);
            else disable();
        }
    }

}
