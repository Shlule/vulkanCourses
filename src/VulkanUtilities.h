#include <iostream>
#include <vector>


struct QueueFamilyIndices{
    int graphicsFamily = -1;
    int presentationFamily = -1;
    bool isValid()
    {
        return graphicsFamily >= 0 && presentationFamily >=0;

    }
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
};

const std::vector<const char*> deviceExtensions
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};