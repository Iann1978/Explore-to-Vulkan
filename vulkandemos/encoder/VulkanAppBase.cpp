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

void VulkanAppBase::showVulkanVersion()
{
	VkResult result = vkEnumerateInstanceVersion(nullptr);
	if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "Vulkan is not supported on this system." << std::endl;

	}

	uint32_t version;
	result = vkEnumerateInstanceVersion(&version);
	if (result != VK_SUCCESS) {
		std::cout << "Failed to get Vulkan version." << std::endl;

	}

	// Step 2: Print Vulkan version  
	uint32_t major = VK_VERSION_MAJOR(version);
	uint32_t minor = VK_VERSION_MINOR(version);
	uint32_t patch = VK_VERSION_PATCH(version);

	std::cout << "Vulkan version: " << major << "." << minor << "." << patch << std::endl;
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


VkDevice VulkanAppBase::createLogicDevice(uint32_t queueFamilyIndex)
{
	std::cout << "createLogicDevice()" << std::endl;



	VkDevice device;


	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 0.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;

	// add extension VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
	//const char* extensions[] = { VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
	//	//, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
	//	, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME 
	//	, VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME 
	//	, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME
	//	, VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME
	//	, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
	//	, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
	//};

	const char* extensions[] = {
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
		VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,
		VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
	};

	createInfo.enabledExtensionCount = 7;
	createInfo.ppEnabledExtensionNames = extensions;



	VkPhysicalDeviceVideoMaintenance1FeaturesKHR videoMaintenance1Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR,
																			 nullptr,
																			 true };

	VkPhysicalDeviceSynchronization2Features synchronization2Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
																		&videoMaintenance1Features,
																		true };


	VkPhysicalDeviceFeatures2 deviceFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &synchronization2Features };
	//GetPhysicalDeviceFeatures2(m_physDevice, &deviceFeatures);
	queueCreateInfo.pNext = &deviceFeatures;


	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logic device!");
	}
	return device;
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

VkQueue VulkanAppBase::pickEncodeQueue(VkPhysicalDevice phyDevice)
{
	auto queryIndex = findQueueFamilyIndex(phyDevice, VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
	if (!queryIndex.has_value()) {
		throw std::runtime_error("failed to find compute queue!");
	}

	VkQueue queue;
	vkGetDeviceQueue(device, queryIndex.value(), 0, &queue);
	return queue;
}

VkQueue VulkanAppBase::pickQueue(VkPhysicalDevice phyDevice, VkQueueFlags queueFlags)
{
	auto queryIndex = findQueueFamilyIndex(phyDevice, queueFlags);
	if (!queryIndex.has_value()) {
		throw std::runtime_error("failed to find compute queue!");
	}

	VkQueue queue;
	vkGetDeviceQueue(device, queryIndex.value(), 0, &queue);
	return queue;
}


VkImage VulkanAppBase::createImage()
{
	std::cout << "  createImage()" << std::endl;
	VkResult result = VK_SUCCESS;
	VkImage image;

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D; // 2D image  
	imageInfo.extent.width = 512;           // Width of the image  
	imageInfo.extent.height = 512;          // Height of the image  
	imageInfo.extent.depth = 1;             // Depth of the image (1 for 2D)  
	imageInfo.mipLevels = 1;                 // Number of mipmap levels  
	imageInfo.arrayLayers = 1;               // Number of array layers  
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // RGB format  
	imageInfo.tiling = VK_IMAGE_TILING_LINEAR; // Optimal tiling for performance  
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout  
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT; // Usage flags  
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Number of samples per pixel  
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Sharing mode  

	result = vkCreateImage(device, &imageInfo, nullptr, &image);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create rgba image!");
	}

	return image;
}


void VulkanAppBase::recordCommandBuffer_TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout newLayout)
{

	// Transition rgba image layout
	VkImageMemoryBarrier imageBarrier = {};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//rgbaImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarrier.newLayout = newLayout;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = image;
	imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageBarrier
	);
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