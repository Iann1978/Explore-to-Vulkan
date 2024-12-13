#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <iostream>
#include <optional>
#include <iostream>
#include <fstream>

class VulkanAppBase
{
public:
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
public:
	VkInstance instance{ VK_NULL_HANDLE };


public: // capibilities
	//std::vector<VkPhysicalDevice> physicalDevices;
	//std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
	//std::vector<VkPhysicalDeviceFeatures> physicalDeviceFeatures;
	//std::vector<VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryProperties;
	//std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	//std::vector<VkExtensionProperties> extensionProperties;
	//std::vector<VkLayerProperties> layerProperties;
	//std::vector<VkSurfaceFormatKHR> surfaceFormats;

	void enumSupportedValidationLayer();
	void enumInstanceExtension();


public:
	void createInstance();
};

