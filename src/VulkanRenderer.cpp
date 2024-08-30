#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(){

}

VulkanRenderer::~VulkanRenderer(){
    
}

int VulkanRenderer::init(GLFWwindow* windowP)
{

    window = windowP;
    try
    {

        createInstance();
        pickPhysicalDevice();

    }
    catch (const std::runtime_error& e)
    {

        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;

    }

    return 0;

}

void VulkanRenderer::createInstance()
{

    // Information about the application
    // This data is for developer convenience
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // Type of the app info
    appInfo.pApplicationName = "Vulkan App"; // Name of the app
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Version of the application
    appInfo.pEngineName = "No Engine"; // Custom engine name
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Custom engine version
    appInfo.apiVersion = VK_API_VERSION_1_1; // Vulkan version (here 1.1)

    // Everything we create will be created with a createInfo
    // Here, info about the vulkan creation
    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // Type of the create info
    // createInfo.pNext // Extended information
    // createInfo.flags // Flags with bitfield
    createInfo.pApplicationInfo = &appInfo; // Application info from above
    // Setup extensions instance will use
    std::vector<const char*> instanceExtensions;
    uint32_t glfwExtensionsCount = 0; // Glfw may require multiple extensions
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    for (size_t i = 0; i < glfwExtensionsCount; ++i)
    {

        instanceExtensions.push_back(glfwExtensions[i]);

    }

    // check Instance
    if (!checkInstanceExtensionSupport(instanceExtensions))
    {

        throw std::runtime_error("VkInstance does not support required extensions");
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    // Validation layers, for now not used
    // TODO : setup
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    // Finally create instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    // Second argument serves to choose where to allocate memory,
    // if you're interested with memory management
    if (result != VK_SUCCESS)
    {

        throw std::runtime_error("Failed to create a Vulkan instance");
    }

}

bool VulkanRenderer::checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions)
{

// How many extensions vulkan supports
uint32_t extensionCount = 0;
// First get the amount of extensions
vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
// Create the vector of extensions

std::vector<VkExtensionProperties> extensions(extensionCount);// A vector with a certain number of elements
// Populate the vector
vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
// This pattern, getting the number of elements then
// populating a vector, is quite common with vulkan
// Check if given extensions are in list of available extensions
    for (const auto& checkExtension : checkExtensions)
    {

    bool hasExtension = false;
    for (const auto& extension : extensions)
    {

        if (strcmp(checkExtension, extension.extensionName) == 0)
        {

            hasExtension = true;
            break;

        }

    }
    if (!hasExtension) return false;

    }

    return true;

}

void VulkanRenderer::clean(){
    vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::pickPhysicalDevice(){
    // Get the number of devices then populate the physical device vector
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    //if no device availlable 
    if (deviceCount ==0)
    {
        throw std::runtime_error("Cannot find any GPU that support vulkan");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data());

    // getdevcie valid for what we want to do
    for(const auto& device : devices){
        if(isDeviceSuitable(device)){
            mainDevice.physicalDevice = device;
            break;
        }
    } 
}

bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice deviceP){
    // information about  the device itself id, name, vendor, etc.
    VkPhysicalDeviceProperties deviceProperties {};
    vkGetPhysicalDeviceProperties(deviceP ,&deviceProperties);

    // information about what the device can do ( geo shader, tesselation, wideline)
    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(deviceP, &deviceFeatures);

    //for now we do nothing with this info
    QueueFamilyIndices indices = getQueueFamilies(deviceP);
    return indices.isValid();

}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice deviceP){
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount =0;
    vkGetPhysicalDeviceQueueFamilyProperties(deviceP, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(deviceP, &queueFamilyCount, queueFamilies.data());

    // go throught each queue family and check it has at least one requires type of queue
    int i =0;
    for(const auto& queueFamily: queueFamilies){
        //check there is at least graphics queue
        if(queueFamily.queueCount >0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        if(indices.isValid()) break;
        ++i;
    }
    return indices;
}