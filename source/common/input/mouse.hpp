#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <functional>

namespace our {

    class Mouse {
    private:
        bool enabled;
        glm::vec2 currentMousePosition, previousMousePosition;
        bool currentMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1], previousMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
        glm::vec2 scrollOffset;

    public:
        void enable(GLFWwindow* window);
        void disable();
        void update();

        [[nodiscard]] const glm::vec2& getMousePosition() const;
        [[nodiscard]] glm::vec2 getMouseDelta() const;
        [[nodiscard]] bool isPressed(int button) const;
        [[nodiscard]] bool justPressed(int button) const;
        [[nodiscard]] bool justReleased(int button) const;
        [[nodiscard]] const glm::vec2& getScrollOffset() const;

        void CursorMoveEvent(double x_position, double y_position);
        void MouseButtonEvent(int button, int action, int mods);
        void ScrollEvent(double x_offset, double y_offset);

        static void lockMouse(GLFWwindow* window);
        static void unlockMouse(GLFWwindow* window);
        static void toggleMouse(GLFWwindow* window);

        [[nodiscard]] bool isEnabled() const;
        void setEnabled(bool enabled, GLFWwindow* window);
    };

}
