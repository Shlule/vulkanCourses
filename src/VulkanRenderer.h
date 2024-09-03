#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <set>
#include "VulkanUtilities.h"


class VulkanRenderer
{
public:

#ifdef NDEBUG
    static const bool enableValidationLayers = false;
#else
    static const bool enableValidationLayers = true;
#endif

    static const std::vector<const char*> validationLayers;

    VulkanRenderer();
    ~VulkanRenderer();
    int init(GLFWwindow* windowP);

    void clean();

private:

    GLFWwindow* window;
    VkInstance instance;

    void createInstance();
    bool checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions);

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice deviceP);
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice deviceP);

    struct {

        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    void createLogicalDevice();

    VkQueue graphicsQueue;

    bool checkValidationLayerSupport();

    std::vector<const char*> getRequiredExtensions();

    VkDebugUtilsMessengerEXT debugMessenger;

    VkResult createDebugUtilsMessengerEXT(VkInstance instanceP, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void destroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void setupDebugMessenger();

    VkSurfaceKHR surface;
    VkQueue presentationQueue;

    void createSurface();
 
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    
};