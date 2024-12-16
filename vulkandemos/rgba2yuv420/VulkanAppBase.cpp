#include "VulkanAppBase.h"


void VulkanAppBase::enumSupportedValidationLayer()
{
	std::cout << "enumSupportedValidationLayer()" << std::endl;
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	// output the supported layers
	std::cout << "available layers:" << std::endl;
	for (const auto& layer : layers)
	{
		std::cout << "\t" << layer.layerName << std::endl;
	}
}

void VulkanAppBase::enumInstanceExtension()
{
	std::cout << "enumInstanceExtension()" << std::endl;
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	// output the supported extensions
	std::cout << "available extensions:" << std::endl;
	for (const auto& extension : extensions)
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}
}



void VulkanAppBase::enumSupportedPipeline(VkPhysicalDevice phyDevice)
{
	std::cout << "enumSupportedPipeline()" << std::endl;
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, extensions.data());
	// output the supported extensions
	std::cout << "available extensions:" << std::endl;
	for (const auto& extension : extensions)
	{
		// filter the extension name with 'pipeline'
		if (std::string(extension.extensionName).find("pipeline") == std::string::npos)
		{
			continue;
		}
		std::cout << "\t" << extension.extensionName << std::endl;
	}
	//VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
}

void VulkanAppBase::enumSupportedQueueFamily(VkPhysicalDevice phyDevice)
{
	std::cout << "enumSupportedQueueFamily()" << std::endl;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, queueFamilies.data());
	// output the supported queue family
	std::cout << "available queue family:" << std::endl;
	for (const auto& queueFamily : queueFamilies)
	{
		std::cout << "\t" << queueFamily.queueCount << " queues with flags: " << queueFamily.queueFlags << std::endl;
	}
}

void VulkanAppBase::createInstance()
{
	std::cout << "createInstance()" << std::endl;
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "rgba2yuv420";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> extensions;
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}