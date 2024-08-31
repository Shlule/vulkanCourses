#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include "VulkanUtilities.h"


class VulkanRenderer
{
public:

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
    

};