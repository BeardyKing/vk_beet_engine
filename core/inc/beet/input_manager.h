#pragma once

#include <beet/subsystem.h>
#include <beet/types.h>
#include <array>

#include <optional>

struct GLFWwindow;

namespace beet {

class Input : public Subsystem {
   public:
    explicit Input();
    ~Input();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    static std::optional<std::reference_wrapper<Input>> get_input();

    //===CURSOR===//
    static void set_cursor_state(MouseState state);

    //===KEYBOARD===//
    bool key_pressed(KeyCode key);
    bool key_held_down(KeyCode key);
    bool key_on_trigger(KeyCode key);
    bool key_on_release(KeyCode key);

    //===MOUSE===//
    bool mouse_button_pressed(MouseButtonCode button);
    bool mouse_button_held_down(MouseButtonCode button);
    bool mouse_button_triggered(MouseButtonCode button);
    bool mouse_button_released(MouseButtonCode button);
    bool mouse_double_clicked(MouseButtonCode button);

    bool wheel_moved();
    vec2 get_relative_position();
    vec2 get_relative_position_raw();
    vec2 get_absolute_position();
    vec2 get_wheel_movement();
    void set_mouse_sensitivity(float sensitivity);
    void set_double_click_limit(float ms);

    void update_relative_positions();

    //===EVENTS===//
    void scroll_event(vec2 offset);
    void key_event(int key, bool pressed);
    void mouse_button_event(int button, bool pressed);
    void mouse_event(double x, double y);
    void update_holds();

    std::array<bool, (size_t)KeyCode::Last> m_keyBinding{};
    std::array<bool, (size_t)KeyCode::Last> m_holdKeyBindings{};
    std::array<bool, (size_t)KeyCode::Last> m_mouseBindings{};
    std::array<bool, (size_t)KeyCode::Last> m_holdMouseBindings{};

    vec2 m_lastScroll;
    vec2d m_mousePosition{};
    vec2d m_lastMousePosition{};
    vec2d m_relativeMousePosition{};
    float m_sensitivity;

   private:
    void setup_callbacks();

    static void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void input_char_callback(GLFWwindow* window, unsigned int codepoint);
    static void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void input_scroll_event(GLFWwindow* window, double xoffset, double yoffset);
    static void input_mouse_event_callback(GLFWwindow* window, double x, double y);
    static void input_cursor_enter_event_callback(GLFWwindow* window, int entered);

   private:
    inline static std::optional<std::reference_wrapper<Input>> s_input = std::nullopt;
};

}  // namespace beet
