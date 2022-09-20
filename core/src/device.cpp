#include <set>

#include <beet/assert.h>
#include <beet/device.h>
#include <beet/engine.h>

namespace beet {

Device::Device(beet::Engine& engine) : m_engine(engine) {}
Device::~Device() {}

void Device::on_awake() {
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

void Device::on_update(double deltaTime) {}
void Device::on_late_update() {}

void Device::on_destroy() {
    vkDestroyDevice(m_device, nullptr);

    if (ENABLED_VALIDATION_LAYERS) {
        destroy_debug_utils_messengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    log::debug("Device destroyed");
}

void Device::create_instance() {
    BEET_ASSERT_MESSAGE(ENABLED_VALIDATION_LAYERS && check_validation_layer_support(),
                        "validation layers requested, but not available!");

    VkApplicationInfo appInformation{};
    appInformation.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInformation.pApplicationName = "vk_beetroot";
    appInformation.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInformation.pEngineName = "beetroot";
    appInformation.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInformation.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInformation{};
    createInformation.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInformation.pApplicationInfo = &appInformation;

    auto extensions = get_required_extensions();
    createInformation.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInformation.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (ENABLED_VALIDATION_LAYERS) {
        createInformation.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInformation.ppEnabledLayerNames = validationLayers.data();

        populate_debug_messenger_create_info(debugCreateInfo);
        createInformation.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInformation.enabledLayerCount = 0;
        createInformation.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInformation, nullptr, &m_instance);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create instance!")
}

QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.is_complete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool Device::is_device_suitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = find_queue_families(device);

    return indices.is_complete();
}

void Device::pick_physical_device() {
    // TODO select device from score + manual select
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    BEET_ASSERT_MESSAGE(deviceCount, "failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (is_device_suitable(device)) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            log::debug("Physical device: {}", properties.deviceName);
            m_physicalDevice = device;
            break;
        }
    }

    BEET_ASSERT_MESSAGE(m_physicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");
}

void Device::create_logical_device() {
    QueueFamilyIndices indices = find_queue_families(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInformation{};
    createInformation.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInformation.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInformation.pQueueCreateInfos = queueCreateInfos.data();

    createInformation.pEnabledFeatures = &deviceFeatures;

    createInformation.enabledExtensionCount = 0;

    if (ENABLED_VALIDATION_LAYERS) {
        createInformation.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInformation.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInformation.enabledLayerCount = 0;
    }

    BEET_ASSERT_MESSAGE(vkCreateDevice(m_physicalDevice, &createInformation, nullptr, &m_device) == VK_SUCCESS,
                        "failed to create logical device!")

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

std::vector<const char*> Device::get_required_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = m_engine.get_window_module().lock()->get_extensions(glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (ENABLED_VALIDATION_LAYERS) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Device::setup_debug_messenger() {
    if (!ENABLED_VALIDATION_LAYERS) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInformation;
    populate_debug_messenger_create_info(createInformation);

    BEET_ASSERT_MESSAGE(
        create_debug_utils_messenger_EXT(m_instance, &createInformation, nullptr, &m_debugMessenger) == VK_SUCCESS,
        "failed to set up debug messenger!")
}

void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInformation) {
    createInformation = {};
    createInformation.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInformation.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInformation.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInformation.pfnUserCallback = debug_callback;
}

bool Device::check_validation_layer_support() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}
void Device::create_surface() {
    m_engine.get_window_module().lock().get()->create_surface(m_instance, m_surface);
}

VkResult create_debug_utils_messenger_EXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messengerEXT(VkInstance instance,
                                      VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                     void* pUserData) {
    std::string prefix{};
    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            prefix = "[GENERAL]: ";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            prefix = "[VALIDATION]: ";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            prefix = "[PERFORMANCE]: ";
            break;
        default:
            prefix = "[UNKNOWN]: ";
            break;
    }
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
#if DEVICE_VERBOSE_LOGGING
            log::debug("{}{}", prefix, pCallbackData->pMessage);
#endif
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
#if DEVICE_VERBOSE_LOGGING
            log::warn("{}{}", prefix, pCallbackData->pMessage);
#endif
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            log::error("{}{}", prefix, pCallbackData->pMessage);
            break;
        default:
            log::info("{}{}", prefix, pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

}  // namespace beet
