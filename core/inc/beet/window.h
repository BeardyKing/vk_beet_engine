#pragma once

#include <vulkan/vulkan.h>

#include <beet/input_manager.h>
#include <beet/subsystem.h>
#include <memory>
#include <string>

namespace beet {
class Engine;
}
class GLFWwindow;

namespace beet {

class Window : public Subsystem {
   public:
    Window(int width, int height, const std::string& title, Engine& engine);
    ~Window();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void swap_frame();
    bool is_open();
    void close();
    void calculate_delta_time();
    double get_delta_time();

    int get_window_width() { return m_width; };
    int get_window_height() { return m_height; };
    vec2i get_window_size() { return vec2i(m_width, m_height); };
    float get_window_aspect_ratio() { return (float)m_width / (float)m_height; }

    void toggle_fullscreen();
    void set_cursor_hide(bool state);

    std::shared_ptr<InputManager> get_input_manager();
    GLFWwindow* get_glfw_window() { return m_window; };
    const char** get_extensions(uint32_t& extensionCount);
    void create_surface(VkInstance& instance, VkSurfaceKHR& surface);

   protected:
    void setup_callbacks();

    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void window_char_callback(GLFWwindow* window, unsigned int codepoint);
    static void window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void window_scroll_event(GLFWwindow* window, double xoffset, double yoffset);
    static void window_mouse_event_callback(GLFWwindow* window, double x, double y);
    static void window_cursor_enter_event_callback(GLFWwindow* window, int entered);
    void toggle_fullscreen_internal();

   protected:
    int m_width;
    int m_height;
    std::string m_title;
    bool m_fullscreen{false};
    bool keysDown = false;

    double m_deltaTime = 0;
    double m_lastFrame = 0;

    static constexpr int WINDOWS_TITLE_BAR_SIZE = 30;
    static constexpr double PHYSICS_TIMESTEP{0.2};
    double m_fixedDeltaTime = PHYSICS_TIMESTEP;

    GLFWwindow* m_window;
    Engine& m_engine;
    std::shared_ptr<InputManager> m_input;
};

}  // namespace beet
