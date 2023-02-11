#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>

#include <beet/input_manager.h>

#include <beet/assert.h>
#include <beet/log.h>
#include <beet/window.h>

#include <algorithm>

namespace beet {

Input::Input() : m_sensitivity(0.3f), m_lastScroll(0.0f) {
    s_input = std::ref(*this);

    m_keyBinding.fill(false);
    m_holdKeyBindings.fill(false);

    m_mouseBindings.fill(false);
    m_holdMouseBindings.fill(false);

    setup_callbacks();
}

void Input::setup_callbacks() {
    GLFWwindow* window = Window::get_window().value().get().get_glfw_window();

    glfwSetKeyCallback(window, Input::input_key_callback);
    glfwSetCharCallback(window, Input::input_char_callback);
    glfwSetMouseButtonCallback(window, Input::input_mouse_button_callback);
    glfwSetScrollCallback(window, Input::input_scroll_event);
    glfwSetCursorPosCallback(window, Input::input_mouse_event_callback);
}

void Input::input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto& input = Input::get_input().value().get();
    if (action != GLFW_REPEAT) {
        input.key_event(key, action != GLFW_RELEASE);
    }
}

void Input::input_char_callback(GLFWwindow* window, unsigned int codepoint) {
    auto& input = Input::get_input().value().get();
    BEET_ASSERT_MESSAGE(BEET_TRUE, "not implemented");
}

void Input::input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Input& input = Input::get_input().value().get();
    input.mouse_button_event(button, action != GLFW_RELEASE);
}

void Input::input_scroll_event(GLFWwindow* window, double xoffset, double yoffset) {
    auto& input = Input::get_input().value().get();
    input.scroll_event({xoffset, yoffset});
}

void Input::input_mouse_event_callback(GLFWwindow* window, double x, double y) {
    if (!glfwGetWindowAttrib(window, GLFW_HOVERED))
        return;
    auto& input = Input::get_input().value().get();
    input.mouse_event(x, y);
}

void Input::input_cursor_enter_event_callback(GLFWwindow* window, int entered) {
    auto& input = Input::get_input().value().get();
    BEET_ASSERT_MESSAGE(BEET_TRUE, "not implemented");
}

void Input::set_cursor_state(MouseState state) {
    GLFWwindow* window = Window::get_window().value().get().get_glfw_window();
    glfwSetInputMode(window, GLFW_CURSOR, (int)state);
}

Input::~Input() {
    log::debug("Input destroyed");
}

void Input::on_awake() {}

void Input::on_update(double deltaTime) {
    update_relative_positions();
    update_holds();
}
void Input::on_late_update() {}
void Input::on_destroy() {}

std::optional<std::reference_wrapper<Input>> Input::get_input() {
    BEET_ASSERT_MESSAGE(s_input.has_value(), "Window does not exist");
    return s_input;
}

/// Start Key Keyboard
bool Input::key_pressed(KeyCode key) {
    return m_keyBinding[static_cast<size_t>(key)];
}

bool Input::key_held_down(KeyCode key) {
    return m_keyBinding[static_cast<size_t>(key)] && m_holdKeyBindings[static_cast<size_t>(key)];
}

bool Input::key_on_trigger(KeyCode key) {
    return m_keyBinding[static_cast<size_t>(key)] && !m_holdKeyBindings[static_cast<size_t>(key)];
}

bool Input::key_on_release(KeyCode key) {
    return m_keyBinding[static_cast<size_t>(key)] && !m_holdKeyBindings[static_cast<size_t>(key)];
}

/// End Of Keyboard

/// Start Of Mouse
bool Input::mouse_button_pressed(MouseButtonCode button) {
    return m_mouseBindings[static_cast<size_t>(button)];
}

bool Input::mouse_button_held_down(MouseButtonCode button) {
    return m_mouseBindings[static_cast<size_t>(button)] && m_holdMouseBindings[static_cast<size_t>(button)];
}

bool Input::mouse_button_triggered(MouseButtonCode button) {
    return m_mouseBindings[static_cast<size_t>(button)] && !m_holdMouseBindings[static_cast<size_t>(button)];
}

bool Input::mouse_button_released(MouseButtonCode button) {
    return !m_mouseBindings[static_cast<size_t>(button)] && m_holdMouseBindings[static_cast<size_t>(button)];
}

bool Input::mouse_double_clicked(MouseButtonCode button) {
    return false;
}

void Input::set_double_click_limit(float ms) {}

bool Input::wheel_moved() {
    return m_lastScroll != vec2{0};
}

vec2 Input::get_relative_position() {
    return {m_relativeMousePosition.x * m_sensitivity, m_relativeMousePosition.y * m_sensitivity};
}

vec2 Input::get_relative_position_raw() {
    return {m_relativeMousePosition.x, m_relativeMousePosition.y};
}

vec2 Input::get_absolute_position() {
    return {m_mousePosition.x, m_mousePosition.y};
}

vec2 Input::get_wheel_movement() {
    return m_lastScroll;
}

void Input::set_mouse_sensitivity(float sens) {
    m_sensitivity = sens;
}
bool input_dirty = true;

void Input::update_relative_positions() {
    if (input_dirty) {
        input_dirty = false;
        m_lastMousePosition = m_mousePosition;
    }
    m_relativeMousePosition = m_mousePosition - m_lastMousePosition;
    m_lastMousePosition = m_mousePosition;
}
/// End of Mouse

/// Start of Events
void Input::scroll_event(vec2 offset) {
    m_lastScroll = offset;
}
void Input::key_event(int key, bool pressed) {
    m_keyBinding[key] = pressed;
}
void Input::mouse_button_event(int button, bool pressed) {
    m_mouseBindings[button] = pressed;
}
void Input::mouse_event(double x, double y) {
    m_mousePosition.x = x;
    m_mousePosition.y = y;
}
/// End of Events

void Input::update_holds() {
    std::copy(std::begin(m_keyBinding), std::end(m_keyBinding), std::begin(m_holdKeyBindings));
    std::copy(std::begin(m_mouseBindings), std::end(m_mouseBindings), std::begin(m_holdMouseBindings));
    m_lastScroll = vec2(0.0f);
}

}  // namespace beet
