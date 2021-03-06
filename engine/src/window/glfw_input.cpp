#include "window/input.h"
#include "platform/platform.h"

ENG_DISABLE_WARNINGS()
#include <GLFW/glfw3.h>
ENG_ENABLE_WARNINGS()

namespace engine
{
    void *Input::m_WindowPointer = nullptr;

    bool Input::IsKeyPressed(const KeyCode key)
    {
        auto *window = reinterpret_cast<GLFWwindow *>(m_WindowPointer);
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(const MouseCode button)
    {
        auto *window = reinterpret_cast<GLFWwindow *>(m_WindowPointer);
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition()
    {
        auto *window = reinterpret_cast<GLFWwindow *>(m_WindowPointer);
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return {(float)xpos, (float)ypos};
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }
}
