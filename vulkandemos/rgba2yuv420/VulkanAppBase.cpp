#include "VulkanAppBase.h"


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}


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

void VulkanAppBase::enumPhysicalDevices(VkInstance instance)
{
	std::cout << "enumPhysicalDevices()" << std::endl;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	// output the supported physical devices
	std::cout << "available physical devices: " << deviceCount << std::endl;
	for (const auto& device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		std::cout << "\t" << deviceProperties.deviceName << std::endl;
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


void VulkanAppBase::pickPhysicalDevice()
{
	std::cout << "pickPhysicalDevice()" << std::endl;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	physicalDevice = devices[0];
}


void VulkanAppBase::createLogicDevice()
{
	std::cout << "createLogicDevice()" << std::endl;

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;

	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logic device!");
	}
}

void VulkanAppBase::createCommandPool()
{
	std::cout << "createCommandPool()" << std::endl;
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = 0;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void VulkanAppBase::pickComputeQueue()
{
	auto queryIndex = findQueueFamilyIndex(physicalDevice, VK_QUEUE_COMPUTE_BIT);
	if (!queryIndex.has_value()) {
		throw std::runtime_error("failed to find compute queue!");
	}

	vkGetDeviceQueue(device, queryIndex.value(), 0, &computeQueue);
}


void VulkanAppBase::setupDebugMessenger() {
	std::cout << "setupDebugMessenger()" << std::endl;
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

std::optional<uint32_t> VulkanAppBase::findQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags queueFlags)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int idx = 0;
	for (const auto& queueFamily : queueFamilies) {
		if ((queueFamily.queueFlags & queueFlags) == queueFlags) {
			return idx;
		}
		idx++;
	}

	return std::nullopt;
}

std::optional<uint32_t> VulkanAppBase::findMemoryTypeIndex(VkPhysicalDevice phyDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(phyDevice, &memoryProperties);
	//uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			return i;
			//memoryTypeIndex = i;
			break;
		}
	}
	return std::nullopt;

}


std::vector<char> VulkanAppBase::loadShaderCode(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}


VkShaderModule VulkanAppBase::createShaderModule(VkDevice device, const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}




VKAPI_ATTR VkBool32 VKAPI_CALL VulkanAppBase::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}