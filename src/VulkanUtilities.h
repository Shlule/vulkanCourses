#pragma once
#include <iostream>
#include <fstream>
#include <vulkan/vulkan.hpp>

#include <vector>
using std::vector;
#include <string>
using std::string;

const vector<const char*> deviceExtensions
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (locations) of Queue Families, if they exist
struct QueueFamilyIndices 
{
	int graphicsFamily = -1;		// Location of Graphics Queue Family
	int presentationFamily = -1;	// Location of Presentation Queue Family

	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData) 
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

struct SwapchainDetails 
{
	// What the surface is capable of displaying, e.g. image size/extent
	VkSurfaceCapabilitiesKHR surfaceCapabilities;		
	// Vector of the image formats, e.g. RGBA
	vector<VkSurfaceFormatKHR> formats;		
	// Vector of presentation modes
	vector<VkPresentModeKHR> presentationModes;			
};

struct SwapchainImage
{

	VkImage image;
	VkImageView imageView;
};

