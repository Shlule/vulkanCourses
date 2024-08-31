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

    // Validation layers,
    if (enableValidationLayers && !checkValidationLayerSupport())
    {

        throw std::runtime_error("validation layers requested, but not available!");
    }
    if (enableValidationLayers)
    {

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

    }
    else
    {

        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

    }
    
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
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
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

void VulkanRenderer::createLogicalDevice(){
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    //queues the logical device needs to create and info to do so
    // one queue for now
    VkDeviceQueueCreateInfo queueCreateInfo {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float priority = 1.0f;

    //Vulkan needs to know how to handle multiple queues. it uses priorities.
    // 1 is the hightest priority
    queueCreateInfo.pQueuePriorities = &priority;

    // logical device creation
    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    //queue info
    deviceCreateInfo.queueCreateInfoCount =1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    //Extension info
    deviceCreateInfo.enabledExtensionCount =0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;

    //--Validation layer are deprecated since Vulkan 1.1
    //features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // Create the logical device for the given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if(result != VK_SUCCESS){
        throw std::runtime_error("Could not create the logical device");
    }

    // Ensure access to queues
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

}

const std::vector<const char*> VulkanRenderer::ValidationLayers{
    "VK_LAYER_KHRONOS_validation"
};

bool VulkanRenderer::checkValidationLayerSupport(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check if all of the layers in validation layers exist in the available layers
    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;
        for( const auto& LayerProperties: availableLayers) 
        {
            if(strcmp(layerName, layerProperties.layerName)==0){
                layerFound = true;
                break;
            }
        }
        if(!layerFound) return false;
        
    }
    return true;
    
}