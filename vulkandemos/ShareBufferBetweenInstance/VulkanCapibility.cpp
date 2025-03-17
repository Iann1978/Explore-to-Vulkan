#include "VulkanCapibility.h"

void VulkanCapibility::showVulkanVersion()
{
	uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	uint32_t major = VK_VERSION_MAJOR(apiVersion);
	uint32_t minor = VK_VERSION_MINOR(apiVersion);
	uint32_t patch = VK_VERSION_PATCH(apiVersion);
	printf("Vulkan Version: %d.%d.%d\n", major, minor, patch);
}

void VulkanCapibility::enumInstanceExternsions()
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions.data());
	bool externalMemorySupported = false;
	for (const auto& extension : extensions) {
		 printf("extension: %s\n", extension.extensionName);
	}
}

void VulkanCapibility::enumDeviceExtensions(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, extensions.data());
	bool externalMemorySupported = false;
	for (const auto& extension : extensions) {
		printf("extension: %s\n", extension.extensionName);
	}
}