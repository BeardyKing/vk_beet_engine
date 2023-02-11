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

   private:
    uint32_t m_width;
    uint32_t m_height;
    std::string m_title;
    bool m_fullscreen = false;

    double m_deltaTime = 0;
    double m_lastFrame = 0;

    static constexpr int WINDOWS_TITLE_BAR_SIZE = 30;
    static constexpr double PHYSICS_TIMESTEP{0.2};
    double m_fixedDeltaTime = PHYSICS_TIMESTEP;

    GLFWwindow* m_window;
    Engine& m_engine;

    inline static std::optional<std::reference_wrapper<Window>> s_window = std::nullopt;
};

}  // namespace beet
