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
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

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
    std::vector<const char*> instanceExtensions = getRequiredExtensions();
    
    // check Instance extensions
    if (!checkInstanceExtensionSupport(instanceExtensions))
    {

        throw std::runtime_error("VkInstance does not support required extensions");
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // Validation layers,
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers && !checkValidationLayerSupport())
    {

        throw std::runtime_error("validation layers requested, but not available!");
    }
    if (enableValidationLayers)
    {

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // setting up the personal debug messenger
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

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
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (enableValidationLayers) {

        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    }
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
    bool extensionSupported = checkDeviceExtensionSupport(deviceP);
    return indices.isValid() && extensionSupported;

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

        // check if queue family support presentation
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(deviceP, i, surface, &presentationSupport);

        if(queueFamily.queueCount >0 && presentationSupport){
            indices.presentationFamily = i;
        }

        if(indices.isValid()) break;
        ++i;
    }
    return indices;
}

void VulkanRenderer::createLogicalDevice(){
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    //Vector for queue creation information and set for family indices
    //A set will only keep one indice if theuy are the same.
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily };

    // queues the logical device needs to create and info to do so.
    for (int queueFamilyIndex : queueFamilyIndices){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        float priority = 1.0f;
        // Vulkan needs to know how to handle multiple queues. It uses priorities.
        // 1 is the hightest priority
        queueCreateInfo.pQueuePriorities = &priority;

        queueCreateInfos.push_back(queueCreateInfo);
        
    }
    // Logical device creation
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //queue Infos 
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    // Extensions info
    // Device extensions, different from instance extensions
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    // -- Validation layers are deprecated since 1.1
    // Features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // Create the logical device for the given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if(result != VK_SUCCESS){
        throw std::runtime_error("Could not create the logical device.");

    }

    //Ensure access to queues
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);


}

const std::vector<const char*> VulkanRenderer::validationLayers{
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
        for( const auto& layerProperties: availableLayers) 
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

std::vector<const char*> VulkanRenderer::getRequiredExtensions(){
    uint32_t glfwExtensionCount =0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}


VkResult VulkanRenderer::createDebugUtilsMessengerEXT(VkInstance instanceP, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger    
)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {

        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

    }
    else
    {

        return VK_ERROR_EXTENSION_NOT_PRESENT;

    }
}

void VulkanRenderer::destroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)

{

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,

    "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
    {

        func(instance, debugMessenger, pAllocator);
    }   

}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
}

void VulkanRenderer::setupDebugMessenger()
{

    if (!enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {

        throw std::runtime_error("Failed to set up debug messenger.");
    }

}

void VulkanRenderer::createSurface(){
    //Create a surface relatively ro our window
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if( result != VK_SUCCESS){
        throw std::runtime_error("failed to create a vulkan surface.");
    }
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    if (extensionCount == 0)
    {

        return false;

    }
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
    for (const auto& deviceExtension : deviceExtensions)
    {

        bool hasExtension = false;
        for (const auto& extension : extensions)
        {

            if (strcmp(deviceExtension, extension.extensionName) == 0)
            {

                hasExtension = true;
                break;

            }

        }
        if (!hasExtension) return false;

    }

    return true;
}

