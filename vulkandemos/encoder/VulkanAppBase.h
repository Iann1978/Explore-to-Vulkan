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
	VkDevice device{ VK_NULL_HANDLE };
	VkCommandPool commandPool{ VK_NULL_HANDLE };
	VkQueue computeQueue{ VK_NULL_HANDLE };

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
	void showVulkanVersion();
	void enumSupportedValidationLayer();
	void enumInstanceExtension();
	void enumPhysicalDevices(VkInstance instance);
	void enumSupportedPipeline(VkPhysicalDevice phyDevice);
	void enumSupportedQueueFamily(VkPhysicalDevice phyDevice);




public:
	void setupDebugMessenger();
	void createInstance();
	void pickPhysicalDevice();
	VkDevice createLogicDevice(uint32_t queueFamilyIndex);
	void createCommandPool();
	void pickComputeQueue();
	VkQueue pickEncodeQueue(VkPhysicalDevice phyDevice);
	VkQueue pickQueue(VkPhysicalDevice phyDevice, VkQueueFlags queueFlags);
	VkImage createImage();
	void recordCommandBuffer_TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout newLayout);
	std::optional<uint32_t> findQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags queueFlags);
	std::optional<uint32_t> findMemoryTypeIndex(VkPhysicalDevice phyDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	std::vector<char> loadShaderCode(const std::string& filename);
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};

