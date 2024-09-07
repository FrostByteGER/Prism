#include "GlfwInputManager.hpp"
#include <stdexcept>
#include <string>
#include "../Utilities/Logging/Log.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Rendering/GlfwWindow.hpp"
#include "../Rendering/Vulkan/ImGui/imgui.h"
#include <GLFW/glfw3.h>

#include "MouseButton.hpp"

Prism::Input::GlfwInputManager::GlfwInputManager(
    const RawPtr<Rendering::IWindowManager> windowManager): IInputManager(windowManager)
{
}

void Prism::Input::GlfwInputManager::initialize()
{
}

void Prism::Input::GlfwInputManager::initializeDeferred()
{
    // We are safe here to cast this to GlfwWindow as this InputManager is only used with GLFW. Nevertheless, still do a sanity check!
    const auto window = dynamic_cast<Rendering::GlfwWindow*>(windowManager->getWindow().get());
    if (!window)
    {
        throw std::runtime_error("Expected the current window to be of type GLFWwindow*!");
    }
    glfwWindow = window->getGLFWWindow();
}

void Prism::Input::GlfwInputManager::initCursorCallback()
{
    // We are safe here to cast this to GlfwWindow as this InputManager is only used with GLFW. Nevertheless, still do a sanity check!
    const auto window = dynamic_cast<Rendering::GlfwWindow*>(windowManager->getWindow().get());
    if (!window)
    {
        throw std::runtime_error("Expected the current window to be of type GLFWwindow*!");
    }
    glfwSetCursorPosCallback(
        window->getGLFWWindow(), [](GLFWwindow* window, const double xPos, const double yPos)
        {
            if (ImGui::GetIO().WantCaptureMouse)
            {
                return;
            }
            size_t dead = 0;
            const auto inputManager = Utility::ServiceLocator::getService<IInputManager>();
            for (auto& callback : inputManager->getMouseMovementCallbacksInternal())
            {
                if (callback)
                {
                    callback(xPos, yPos);
                }
                else
                {
                    ++dead;
                }
            }
            inputManager->setDeadMouseCallbacksInternal(dead);
        });
}

void Prism::Input::GlfwInputManager::deInitialize()
{
}

void Prism::Input::GlfwInputManager::registerMouseMovementInput(
    const std::function<void(double xPos, double yPos)>& callback)
{
    mouseMovementCallbacks.emplace_back(callback);
}

bool Prism::Input::GlfwInputManager::isKeyDown(const InputKey key) const
{
    return isKeyPressed(key);
}

bool Prism::Input::GlfwInputManager::isKeyPressed(const InputKey key) const
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return false;
    }
    return glfwGetKey(glfwWindow, mapInputKeyToGlfwKey(key)) == GLFW_PRESS;
}

bool Prism::Input::GlfwInputManager::isKeyReleased(const InputKey key) const
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return false;
    }
    return glfwGetKey(glfwWindow, mapInputKeyToGlfwKey(key)) == GLFW_RELEASE;
}

bool Prism::Input::GlfwInputManager::isMouseButtonDown(const MouseButton button) const
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return false;
    }
    return glfwGetMouseButton(glfwWindow, mapMouseButtonToGlfwButton(button)) == GLFW_PRESS;
}

bool Prism::Input::GlfwInputManager::isMouseButtonReleased(const MouseButton button) const
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return false;
    }
    return glfwGetMouseButton(glfwWindow, mapMouseButtonToGlfwButton(button)) == GLFW_RELEASE;
}

void Prism::Input::GlfwInputManager::cleanupDeadCallbacks()
{
    const size_t erasedMouseMovementCallbacks = std::erase_if(mouseMovementCallbacks,
                                                              [](const std::function<void(double, double)>& callback)
                                                              {
                                                                  return callback == nullptr;
                                                              });
    LOG_DEBUG("Removed " + std::to_string(erasedMouseMovementCallbacks) + " dead mouseMovementCallbacks!");
}

Prism::Input::InputKey Prism::Input::GlfwInputManager::mapGlfwKeyToInputKey(int glfwKey) const
{
    switch (glfwKey)
    {
    case GLFW_KEY_UNKNOWN:
        return InputKey::UNKNOWN;
    case GLFW_KEY_SPACE:
        return InputKey::SPACE;
    case GLFW_KEY_APOSTROPHE:
        return InputKey::APOSTROPHE;
    case GLFW_KEY_COMMA:
        return InputKey::COMMA;
    case GLFW_KEY_MINUS:
        return InputKey::MINUS;
    case GLFW_KEY_PERIOD:
        return InputKey::PERIOD;
    case GLFW_KEY_SLASH:
        return InputKey::SLASH;
    case GLFW_KEY_0:
        return InputKey::NUMBER_0;
    case GLFW_KEY_1:
        return InputKey::NUMBER_1;
    case GLFW_KEY_2:
        return InputKey::NUMBER_2;
    case GLFW_KEY_3:
        return InputKey::NUMBER_3;
    case GLFW_KEY_4:
        return InputKey::NUMBER_4;
    case GLFW_KEY_5:
        return InputKey::NUMBER_5;
    case GLFW_KEY_6:
        return InputKey::NUMBER_6;
    case GLFW_KEY_7:
        return InputKey::NUMBER_7;
    case GLFW_KEY_8:
        return InputKey::NUMBER_8;
    case GLFW_KEY_9:
        return InputKey::NUMBER_9;
    case GLFW_KEY_SEMICOLON:
        return InputKey::SEMICOLON;
    case GLFW_KEY_EQUAL:
        return InputKey::EQUAL;
    case GLFW_KEY_A:
        return InputKey::A;
    case GLFW_KEY_B:
        return InputKey::B;
    case GLFW_KEY_C:
        return InputKey::C;
    case GLFW_KEY_D:
        return InputKey::D;
    case GLFW_KEY_E:
        return InputKey::E;
    case GLFW_KEY_F:
        return InputKey::F;
    case GLFW_KEY_G:
        return InputKey::G;
    case GLFW_KEY_H:
        return InputKey::H;
    case GLFW_KEY_I:
        return InputKey::I;
    case GLFW_KEY_J:
        return InputKey::J;
    case GLFW_KEY_K:
        return InputKey::K;
    case GLFW_KEY_L:
        return InputKey::L;
    case GLFW_KEY_M:
        return InputKey::M;
    case GLFW_KEY_N:
        return InputKey::N;
    case GLFW_KEY_O:
        return InputKey::O;
    case GLFW_KEY_P:
        return InputKey::P;
    case GLFW_KEY_Q:
        return InputKey::Q;
    case GLFW_KEY_R:
        return InputKey::R;
    case GLFW_KEY_S:
        return InputKey::S;
    case GLFW_KEY_T:
        return InputKey::T;
    case GLFW_KEY_U:
        return InputKey::U;
    case GLFW_KEY_V:
        return InputKey::V;
    case GLFW_KEY_W:
        return InputKey::W;
    case GLFW_KEY_X:
        return InputKey::X;
    case GLFW_KEY_Y:
        return InputKey::Y;
    case GLFW_KEY_Z:
        return InputKey::Z;
    case GLFW_KEY_LEFT_BRACKET:
        return InputKey::LEFT_BRACKET;
    case GLFW_KEY_BACKSLASH:
        return InputKey::BACKSLASH;
    case GLFW_KEY_RIGHT_BRACKET:
        return InputKey::RIGHT_BRACKET;
    case GLFW_KEY_GRAVE_ACCENT:
        return InputKey::GRAVE_ACCENT;
    case GLFW_KEY_WORLD_1:
        return InputKey::WORLD_1;
    case GLFW_KEY_WORLD_2:
        return InputKey::WORLD_2;
    case GLFW_KEY_ESCAPE:
        return InputKey::ESCAPE;
    case GLFW_KEY_ENTER:
        return InputKey::ENTER;
    case GLFW_KEY_TAB:
        return InputKey::TAB;
    case GLFW_KEY_BACKSPACE:
        return InputKey::BACKSPACE;
    case GLFW_KEY_INSERT:
        return InputKey::INSERT;
    case GLFW_KEY_DELETE:
        return InputKey::DEL;
    case GLFW_KEY_RIGHT:
        return InputKey::RIGHT;
    case GLFW_KEY_LEFT:
        return InputKey::LEFT;
    case GLFW_KEY_DOWN:
        return InputKey::DOWN;
    case GLFW_KEY_UP:
        return InputKey::UP;
    case GLFW_KEY_PAGE_UP:
        return InputKey::PAGE_UP;
    case GLFW_KEY_PAGE_DOWN:
        return InputKey::PAGE_DOWN;
    case GLFW_KEY_HOME:
        return InputKey::HOME;
    case GLFW_KEY_END:
        return InputKey::END;
    case GLFW_KEY_CAPS_LOCK:
        return InputKey::CAPS_LOCK;
    case GLFW_KEY_SCROLL_LOCK:
        return InputKey::SCROLL_LOCK;
    case GLFW_KEY_NUM_LOCK:
        return InputKey::NUM_LOCK;
    case GLFW_KEY_PRINT_SCREEN:
        return InputKey::PRINT_SCREEN;
    case GLFW_KEY_PAUSE:
        return InputKey::PAUSE;
    case GLFW_KEY_F1:
        return InputKey::F1;
    case GLFW_KEY_F2:
        return InputKey::F2;
    case GLFW_KEY_F3:
        return InputKey::F3;
    case GLFW_KEY_F4:
        return InputKey::F4;
    case GLFW_KEY_F5:
        return InputKey::F5;
    case GLFW_KEY_F6:
        return InputKey::F6;
    case GLFW_KEY_F7:
        return InputKey::F7;
    case GLFW_KEY_F8:
        return InputKey::F8;
    case GLFW_KEY_F9:
        return InputKey::F9;
    case GLFW_KEY_F10:
        return InputKey::F10;
    case GLFW_KEY_F11:
        return InputKey::F11;
    case GLFW_KEY_F12:
        return InputKey::F12;
    case GLFW_KEY_F13:
        return InputKey::F13;
    case GLFW_KEY_F14:
        return InputKey::F14;
    case GLFW_KEY_F15:
        return InputKey::F15;
    case GLFW_KEY_F16:
        return InputKey::F16;
    case GLFW_KEY_F17:
        return InputKey::F17;
    case GLFW_KEY_F18:
        return InputKey::F18;
    case GLFW_KEY_F19:
        return InputKey::F19;
    case GLFW_KEY_F20:
        return InputKey::F20;
    case GLFW_KEY_F21:
        return InputKey::F21;
    case GLFW_KEY_F22:
        return InputKey::F22;
    case GLFW_KEY_F23:
        return InputKey::F23;
    case GLFW_KEY_F24:
        return InputKey::F24;
    case GLFW_KEY_F25:
        return InputKey::F25;
    case GLFW_KEY_KP_0:
        return InputKey::KP_0;
    case GLFW_KEY_KP_1:
        return InputKey::KP_1;
    case GLFW_KEY_KP_2:
        return InputKey::KP_2;
    case GLFW_KEY_KP_3:
        return InputKey::KP_3;
    case GLFW_KEY_KP_4:
        return InputKey::KP_4;
    case GLFW_KEY_KP_5:
        return InputKey::KP_5;
    case GLFW_KEY_KP_6:
        return InputKey::KP_6;
    case GLFW_KEY_KP_7:
        return InputKey::KP_7;
    case GLFW_KEY_KP_8:
        return InputKey::KP_8;
    case GLFW_KEY_KP_9:
        return InputKey::KP_9;
    case GLFW_KEY_KP_DECIMAL:
        return InputKey::KP_DECIMAL;
    case GLFW_KEY_KP_DIVIDE:
        return InputKey::KP_DIVIDE;
    case GLFW_KEY_KP_MULTIPLY:
        return InputKey::KP_MULTIPLY;
    case GLFW_KEY_KP_SUBTRACT:
        return InputKey::KP_SUBTRACT;
    case GLFW_KEY_KP_ADD:
        return InputKey::KP_ADD;
    case GLFW_KEY_KP_ENTER:
        return InputKey::KP_ENTER;
    case GLFW_KEY_KP_EQUAL:
        return InputKey::KP_EQUAL;
    case GLFW_KEY_LEFT_SHIFT:
        return InputKey::LEFT_SHIFT;
    case GLFW_KEY_LEFT_CONTROL:
        return InputKey::LEFT_CONTROL;
    case GLFW_KEY_LEFT_ALT:
        return InputKey::LEFT_ALT;
    case GLFW_KEY_LEFT_SUPER:
        return InputKey::LEFT_SUPER;
    case GLFW_KEY_RIGHT_SHIFT:
        return InputKey::RIGHT_SHIFT;
    case GLFW_KEY_RIGHT_CONTROL:
        return InputKey::RIGHT_CONTROL;
    case GLFW_KEY_RIGHT_ALT:
        return InputKey::RIGHT_ALT;
    case GLFW_KEY_RIGHT_SUPER:
        return InputKey::RIGHT_SUPER;
    case GLFW_KEY_MENU:
        return InputKey::MENU;
    default:
        return InputKey::UNKNOWN;
    }
}

int Prism::Input::GlfwInputManager::mapInputKeyToGlfwKey(InputKey inputKey) const
{
    switch (inputKey)
    {
    case InputKey::UNKNOWN:
        return GLFW_KEY_UNKNOWN;
    case InputKey::SPACE:
        return GLFW_KEY_SPACE;
    case InputKey::APOSTROPHE:
        return GLFW_KEY_APOSTROPHE;
    case InputKey::COMMA:
        return GLFW_KEY_COMMA;
    case InputKey::MINUS:
        return GLFW_KEY_MINUS;
    case InputKey::PERIOD:
        return GLFW_KEY_PERIOD;
    case InputKey::SLASH:
        return GLFW_KEY_SLASH;
    case InputKey::NUMBER_0:
        return GLFW_KEY_0;
    case InputKey::NUMBER_1:
        return GLFW_KEY_1;
    case InputKey::NUMBER_2:
        return GLFW_KEY_2;
    case InputKey::NUMBER_3:
        return GLFW_KEY_3;
    case InputKey::NUMBER_4:
        return GLFW_KEY_4;
    case InputKey::NUMBER_5:
        return GLFW_KEY_5;
    case InputKey::NUMBER_6:
        return GLFW_KEY_6;
    case InputKey::NUMBER_7:
        return GLFW_KEY_7;
    case InputKey::NUMBER_8:
        return GLFW_KEY_8;
    case InputKey::NUMBER_9:
        return GLFW_KEY_9;
    case InputKey::SEMICOLON:
        return GLFW_KEY_SEMICOLON;
    case InputKey::EQUAL:
        return GLFW_KEY_EQUAL;
    case InputKey::A:
        return GLFW_KEY_A;
    case InputKey::B:
        return GLFW_KEY_B;
    case InputKey::C:
        return GLFW_KEY_C;
    case InputKey::D:
        return GLFW_KEY_D;
    case InputKey::E:
        return GLFW_KEY_E;
    case InputKey::F:
        return GLFW_KEY_F;
    case InputKey::G:
        return GLFW_KEY_G;
    case InputKey::H:
        return GLFW_KEY_H;
    case InputKey::I:
        return GLFW_KEY_I;
    case InputKey::J:
        return GLFW_KEY_J;
    case InputKey::K:
        return GLFW_KEY_K;
    case InputKey::L:
        return GLFW_KEY_L;
    case InputKey::M:
        return GLFW_KEY_M;
    case InputKey::N:
        return GLFW_KEY_N;
    case InputKey::O:
        return GLFW_KEY_O;
    case InputKey::P:
        return GLFW_KEY_P;
    case InputKey::Q:
        return GLFW_KEY_Q;
    case InputKey::R:
        return GLFW_KEY_R;
    case InputKey::S:
        return GLFW_KEY_S;
    case InputKey::T:
        return GLFW_KEY_T;
    case InputKey::U:
        return GLFW_KEY_U;
    case InputKey::V:
        return GLFW_KEY_V;
    case InputKey::W:
        return GLFW_KEY_W;
    case InputKey::X:
        return GLFW_KEY_X;
    case InputKey::Y:
        return GLFW_KEY_Y;
    case InputKey::Z:
        return GLFW_KEY_Z;
    case InputKey::LEFT_BRACKET:
        return GLFW_KEY_LEFT_BRACKET;
    case InputKey::BACKSLASH:
        return GLFW_KEY_BACKSLASH;
    case InputKey::RIGHT_BRACKET:
        return GLFW_KEY_RIGHT_BRACKET;
    case InputKey::GRAVE_ACCENT:
        return GLFW_KEY_GRAVE_ACCENT;
    case InputKey::WORLD_1:
        return GLFW_KEY_WORLD_1;
    case InputKey::WORLD_2:
        return GLFW_KEY_WORLD_2;
    case InputKey::ESCAPE:
        return GLFW_KEY_ESCAPE;
    case InputKey::ENTER:
        return GLFW_KEY_ENTER;
    case InputKey::TAB:
        return GLFW_KEY_TAB;
    case InputKey::BACKSPACE:
        return GLFW_KEY_BACKSPACE;
    case InputKey::INSERT:
        return GLFW_KEY_INSERT;
    case InputKey::DEL:
        return GLFW_KEY_DELETE;
    case InputKey::RIGHT:
        return GLFW_KEY_RIGHT;
    case InputKey::LEFT:
        return GLFW_KEY_LEFT;
    case InputKey::DOWN:
        return GLFW_KEY_DOWN;
    case InputKey::UP:
        return GLFW_KEY_UP;
    case InputKey::PAGE_UP:
        return GLFW_KEY_PAGE_UP;
    case InputKey::PAGE_DOWN:
        return GLFW_KEY_PAGE_DOWN;
    case InputKey::HOME:
        return GLFW_KEY_HOME;
    case InputKey::END:
        return GLFW_KEY_END;
    case InputKey::CAPS_LOCK:
        return GLFW_KEY_CAPS_LOCK;
    case InputKey::SCROLL_LOCK:
        return GLFW_KEY_SCROLL_LOCK;
    case InputKey::NUM_LOCK:
        return GLFW_KEY_NUM_LOCK;
    case InputKey::PRINT_SCREEN:
        return GLFW_KEY_PRINT_SCREEN;
    case InputKey::PAUSE:
        return GLFW_KEY_PAUSE;
    case InputKey::F1:
        return GLFW_KEY_F1;
    case InputKey::F2:
        return GLFW_KEY_F2;
    case InputKey::F3:
        return GLFW_KEY_F3;
    case InputKey::F4:
        return GLFW_KEY_F4;
    case InputKey::F5:
        return GLFW_KEY_F5;
    case InputKey::F6:
        return GLFW_KEY_F6;
    case InputKey::F7:
        return GLFW_KEY_F7;
    case InputKey::F8:
        return GLFW_KEY_F8;
    case InputKey::F9:
        return GLFW_KEY_F9;
    case InputKey::F10:
        return GLFW_KEY_F10;
    case InputKey::F11:
        return GLFW_KEY_F11;
    case InputKey::F12:
        return GLFW_KEY_F12;
    case InputKey::F13:
        return GLFW_KEY_F13;
    case InputKey::F14:
        return GLFW_KEY_F14;
    case InputKey::F15:
        return GLFW_KEY_F15;
    case InputKey::F16:
        return GLFW_KEY_F16;
    case InputKey::F17:
        return GLFW_KEY_F17;
    case InputKey::F18:
        return GLFW_KEY_F18;
    case InputKey::F19:
        return GLFW_KEY_F19;
    case InputKey::F20:
        return GLFW_KEY_F20;
    case InputKey::F21:
        return GLFW_KEY_F21;
    case InputKey::F22:
        return GLFW_KEY_F22;
    case InputKey::F23:
        return GLFW_KEY_F23;
    case InputKey::F24:
        return GLFW_KEY_F24;
    case InputKey::F25:
        return GLFW_KEY_F25;
    case InputKey::KP_0:
        return GLFW_KEY_KP_0;
    case InputKey::KP_1:
        return GLFW_KEY_KP_1;
    case InputKey::KP_2:
        return GLFW_KEY_KP_2;
    case InputKey::KP_3:
        return GLFW_KEY_KP_3;
    case InputKey::KP_4:
        return GLFW_KEY_KP_4;
    case InputKey::KP_5:
        return GLFW_KEY_KP_5;
    case InputKey::KP_6:
        return GLFW_KEY_KP_6;
    case InputKey::KP_7:
        return GLFW_KEY_KP_7;
    case InputKey::KP_8:
        return GLFW_KEY_KP_8;
    case InputKey::KP_9:
        return GLFW_KEY_KP_9;
    case InputKey::KP_DECIMAL:
        return GLFW_KEY_KP_DECIMAL;
    case InputKey::KP_DIVIDE:
        return GLFW_KEY_KP_DIVIDE;
    case InputKey::KP_MULTIPLY:
        return GLFW_KEY_KP_MULTIPLY;
    case InputKey::KP_SUBTRACT:
        return GLFW_KEY_KP_SUBTRACT;
    case InputKey::KP_ADD:
        return GLFW_KEY_KP_ADD;
    case InputKey::KP_ENTER:
        return GLFW_KEY_KP_ENTER;
    case InputKey::KP_EQUAL:
        return GLFW_KEY_KP_EQUAL;
    case InputKey::LEFT_SHIFT:
        return GLFW_KEY_LEFT_SHIFT;
    case InputKey::LEFT_CONTROL:
        return GLFW_KEY_LEFT_CONTROL;
    case InputKey::LEFT_ALT:
        return GLFW_KEY_LEFT_ALT;
    case InputKey::LEFT_SUPER:
        return GLFW_KEY_LEFT_SUPER;
    case InputKey::RIGHT_SHIFT:
        return GLFW_KEY_RIGHT_SHIFT;
    case InputKey::RIGHT_CONTROL:
        return GLFW_KEY_RIGHT_CONTROL;
    case InputKey::RIGHT_ALT:
        return GLFW_KEY_RIGHT_ALT;
    case InputKey::RIGHT_SUPER:
        return GLFW_KEY_RIGHT_SUPER;
    case InputKey::MENU:
        return GLFW_KEY_MENU;
    default:
        return GLFW_KEY_UNKNOWN;
    }
}

int Prism::Input::GlfwInputManager::mapMouseButtonToGlfwButton(const MouseButton button) const
{
    switch (button)
    {
    case MouseButton::LEFT:
        return GLFW_MOUSE_BUTTON_LEFT;
    case MouseButton::RIGHT:
        return GLFW_MOUSE_BUTTON_RIGHT;
    case MouseButton::MIDDLE:
        return GLFW_MOUSE_BUTTON_MIDDLE;
    case MouseButton::BUTTON_4:
        return GLFW_MOUSE_BUTTON_4;
    case MouseButton::BUTTON_5:
        return GLFW_MOUSE_BUTTON_5;
    case MouseButton::BUTTON_6:
        return GLFW_MOUSE_BUTTON_6;
    case MouseButton::BUTTON_7:
        return GLFW_MOUSE_BUTTON_7;
    case MouseButton::BUTTON_8:
        return GLFW_MOUSE_BUTTON_8;
    default:
        return GLFW_MOUSE_BUTTON_LEFT;
    }
}
