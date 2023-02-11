#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/window.h>

namespace beet {

Window::Window(int width, int height, const std::string& title, Engine& engine)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr), m_engine(engine) {
    s_window = std::ref(*this);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow((int)m_width, (int)m_height, m_title.c_str(), nullptr, nullptr);
    glfwSetWindowSizeLimits(m_window, 128, 128, GLFW_DONT_CARE, GLFW_DONT_CARE);

    BEET_ASSERT_MESSAGE(m_window, "Failed to create GLFW window");
    log::debug("GLFW initialized");

    glfwSetWindowMonitor(m_window, nullptr, 0, WINDOWS_TITLE_BAR_SIZE, (int)m_width, (int)m_height, 1);
    glfwMakeContextCurrent(m_window);
}

void Window::on_awake() {
    glfwSetWindowUserPointer(m_window, this);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSwapInterval(1);
    setup_callbacks();
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    auto& renderer = Renderer::get_renderer().value().get();
    auto* self = (Window*)glfwGetWindowUserPointer(window);

    vec2i size{width, height};
    self->m_width = size.x;
    self->m_height = size.y;

    renderer.recreate_swap_chain();
}

bool Window::is_open() {
    return !glfwWindowShouldClose(m_window);
}

void Window::close() {
    glfwSetWindowShouldClose(m_window, true);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    log::debug("Window destroyed");
}

void Window::on_update(double deltaTime) {
    glfwPollEvents();
    calculate_delta_time();
    toggle_fullscreen();
}

void Window::toggle_fullscreen() {
    auto& input = Input::get_input().value().get();
    if (input.key_held_down(KeyCode::LeftAlt) && input.key_on_trigger(KeyCode::Enter)) {
        toggle_fullscreen_internal();
    }
}

void Window::toggle_fullscreen_internal() {
    vec2i windowPos;
    glfwGetWindowPos(m_window, &windowPos.x, &windowPos.y);
    m_fullscreen = !m_fullscreen;
    if (m_fullscreen) {
        // best window selection from https://stackoverflow.com/a/31526753
        int nmonitors, i;
        int wx, wy, ww, wh;
        int mx, my, mw, mh;
        int overlap, bestoverlap;
        GLFWmonitor* bestmonitor;
        GLFWmonitor** monitors;
        const GLFWvidmode* mode;

        bestoverlap = 0;
        bestmonitor = NULL;

        glfwGetWindowPos(m_window, &wx, &wy);
        glfwGetWindowSize(m_window, &ww, &wh);
        monitors = glfwGetMonitors(&nmonitors);

        for (i = 0; i < nmonitors; i++) {
            mode = glfwGetVideoMode(monitors[i]);
            glfwGetMonitorPos(monitors[i], &mx, &my);
            mw = mode->width;
            mh = mode->height;

            overlap = max(0, min(wx + ww, mx + mw) - max(wx, mx)) * max(0, min(wy + wh, my + mh) - max(wy, my));

            if (bestoverlap < overlap) {
                bestoverlap = overlap;
                bestmonitor = monitors[i];
            }
        }
        auto bestMode = glfwGetVideoMode(bestmonitor);

        m_width = bestMode->width;
        m_height = bestMode->height;

        glfwSetWindowMonitor(m_window, bestmonitor, windowPos.x, 0, bestMode->width, bestMode->height,
                             bestMode->refreshRate);
    } else if (!m_fullscreen) {
        glfwSetWindowMonitor(m_window, NULL, windowPos.x, windowPos.y + WINDOWS_TITLE_BAR_SIZE, m_width, m_height, 0);
    }

    log::debug("Window toggle size : {}, {}", m_width, m_height);
}

void Window::calculate_delta_time() {
    double currentFrame = glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}

double Window::get_delta_time() {
    return m_deltaTime;
}

void Window::on_late_update() {}
void Window::on_destroy() {}
void Window::swap_frame() {
    glfwSwapBuffers(m_window);
}

void Window::setup_callbacks() {
    glfwSetWindowSizeCallback(m_window, Window::window_size_callback);

    //    glfwSetKeyCallback(m_window, Window::window_key_callback);
    //    glfwSetCharCallback(m_window, Window::window_char_callback);
    //    glfwSetMouseButtonCallback(m_window, Window::window_mouse_button_callback);
    //    glfwSetScrollCallback(m_window, Window::window_scroll_event);
    //    glfwSetCursorPosCallback(m_window, Window::window_mouse_event_callback);
}

void Window::wait_events() {
    glfwWaitEvents();
}

vec2u Window::get_window_size_internal() {
    return vec2u{m_width, m_height};
};

float Window::get_window_aspect_ratio_internal() {
    return (float)m_width / (float)m_height;
}

void Window::get_framebuffer_size_internal(vec2i& size) {
    glfwGetFramebufferSize(m_window, &size.x, &size.y);
    m_width = size.x;
    m_height = size.y;
}

void Window::create_surface_internal(VkInstance& instance, VkSurfaceKHR& surface) {
    VkResult windowResult = glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
    BEET_ASSERT_MESSAGE(windowResult == VK_SUCCESS, "failed to create window surface!");
}

std::optional<std::reference_wrapper<Window>> Window::get_window() {
    BEET_ASSERT_MESSAGE(s_window.has_value(), "Window does not exist")
    return s_window;
}

vec2u Window::get_size() {
    return get_window().value().get().get_window_size_internal();
}
float Window::get_aspect_ratio() {
    return get_window().value().get().get_window_aspect_ratio_internal();
}
void Window::get_framebuffer_size(vec2i& size) {
    get_window().value().get().get_framebuffer_size_internal(size);
}
void Window::create_surface(VkInstance& instance, VkSurfaceKHR& surface) {
    get_window().value().get().create_surface_internal(instance, surface);
}

}  // namespace beet
