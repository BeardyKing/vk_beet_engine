#pragma once

#include <vulkan/vulkan.h>

#include <beet/input_manager.h>
#include <beet/subsystem.h>

#include <memory>
#include <optional>
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

    static std::optional<std::reference_wrapper<Window>> get_window();
    GLFWwindow* get_glfw_window() { return m_window; };

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    static vec2u get_size();
    static float get_aspect_ratio();
    static void get_framebuffer_size(vec2i& size);
    static void create_surface(VkInstance& instance, VkSurfaceKHR& surface);

    void wait_events();
    void swap_frame();

    void set_cursor_hide(bool state);
    void toggle_fullscreen();
    bool is_open();
    void close();
    double get_delta_time();

   private:
    void calculate_delta_time();
    void toggle_fullscreen_internal();

    vec2u get_window_size_internal();
    float get_window_aspect_ratio_internal();
    void get_framebuffer_size_internal(vec2i& size);
    void create_surface_internal(VkInstance& instance, VkSurfaceKHR& surface);

   private:
    void setup_callbacks();

    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void window_char_callback(GLFWwindow* window, unsigned int codepoint);
    static void window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void window_scroll_event(GLFWwindow* window, double xoffset, double yoffset);
    static void window_mouse_event_callback(GLFWwindow* window, double x, double y);
    static void window_cursor_enter_event_callback(GLFWwindow* window, int entered);

   private:
    uint32_t m_width;
    uint32_t m_height;
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

    inline static std::optional<std::reference_wrapper<Window>> s_window = std::nullopt;

    std::shared_ptr<InputManager> m_input;
};

}  // namespace beet
