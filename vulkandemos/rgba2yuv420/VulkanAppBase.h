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
	VkDebugUtilsMessengerEXT debugMessenger;
	VkInstance instance{ VK_NULL_HANDLE };
	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

public: // capibilities
	//std::vector<VkPhysicalDevice> physicalDevices;
	//std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
	//std::vector<VkPhysicalDeviceFeatures> physicalDeviceFeatures;
	//std::vector<VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryProperties;
	//std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	//std::vector<VkExtensionProperties> extensionProperties;
	//std::vector<VkLayerProperties> layerProperties;
	//std::vector<VkSurfaceFormatKHR> surfaceFormats;
	
	// enumerate the capibilities
	void enumSupportedValidationLayer();
	void enumInstanceExtension();
	void enumPhysicalDevices(VkInstance instance);
	void enumSupportedPipeline(VkPhysicalDevice phyDevice);
	void enumSupportedQueueFamily(VkPhysicalDevice phyDevice);


public:
	void setupDebugMessenger();
	void createInstance();
	void pickPhysicalDevice();
	std::optional<uint32_t> findQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags queueFlags);
	std::optional<uint32_t> findMemoryTypeIndex(VkPhysicalDevice phyDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	std::vector<char> loadShaderCode(const std::string& filename);
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};

