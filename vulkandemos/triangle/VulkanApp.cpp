#include "VulkanApp.h"

// rgba2yuv420.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(lib, "vulkan-1.lib")

#define STB_IMAGE_WRITE_IMPLEMENTATION  

#include "stb_image_write.h" 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

void VulkanApp::enumSupportedValidationLayer()
{
	StackLog _(logStack, __FUNCTION__);

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

void VulkanApp::run()
{
	StackLog _(logStack, __FUNCTION__);
	//std::cout << "run()" << std::endl;
	initWindow();
	initVulkan();
	mainLoop();
	//cleanup();
}

void VulkanApp::initWindow()
{
	StackLog _(logStack, __FUNCTION__);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
}

void VulkanApp::initVulkan()
{
	StackLog _(logStack, __FUNCTION__);

	enumSupportedValidationLayer();
	createInstance();
	setupDebugMessenger();
	physicalDevice = pickPhysicalDevice();
	device = createLogicDevice();

	renderTarget = new RenderTarget(window, instance, device, logStack);
	renderTarget->surface = renderTarget->createSurface();
	renderTarget->swapChain = renderTarget->createSwapChain();
	renderTarget->getSwapChainImages();
	for (auto image : renderTarget->swapChainImages)
	{
		renderTarget->swapChainImageViews.push_back(renderTarget->createImageView(image));
	}
	renderTarget->renderPass = createRenderPass();
	renderTarget->createFramebuffers();

	graphicQueue = pickGraphicQueue();
	presentQueue = pickPresentQueue();

	shader = new Shader(device, renderTarget->renderPass, "triangle.vert.spv", "triangle.frag.spv", logStack);
	material = new Material(this->physicalDevice, this->device,shader, logStack);

	createCommandPool();
	createCommandBuffer();

	recordCommandBuffer(commandBuffer, 0);
	createFence();
	int a = 0;
	int b = 0;
}

void VulkanApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	StackLog _(logStack, __FUNCTION__);
	//std::cout << "    recordCommandBuffer()" << std::endl;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderTarget->renderPass;
	renderPassInfo.framebuffer = renderTarget->framebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = renderTarget->swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	material->Bind(commandBuffer);
	
	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->graphicsPipeline);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);

	vkEndCommandBuffer(commandBuffer);


}

VkRenderPass VulkanApp::createRenderPass()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "createRenderPass()" << std::endl;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;


	VkRenderPass renderPass{ VK_NULL_HANDLE };
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
	return renderPass;

}

VkQueue VulkanApp::pickGraphicQueue()
{
	StackLog _(logStack, __FUNCTION__);


	//std::cout << "    pickGraphicQueue()" << std::endl;
	int queryFamilyIndex = 0;
	VkQueue graphicsQueue;
	vkGetDeviceQueue(device, queryFamilyIndex, 0, &graphicsQueue);
	return graphicsQueue;
}

VkQueue VulkanApp::pickPresentQueue()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "    pickPresentQueue()" << std::endl;
	int queryFamilyIndex = 0;
	VkQueue presentQueue;
	vkGetDeviceQueue(device, queryFamilyIndex, 0, &presentQueue);
	return presentQueue;
}

VkDevice VulkanApp::createLogicDevice()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "createLogicDevice()" << std::endl;



	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	std::vector<const char*> deviceExtensions;
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VkDevice device;
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logic device!");
	}
	return device;
}

VkPhysicalDevice VulkanApp::pickPhysicalDevice()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "  pickPhysicalDevice()" << std::endl;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	return devices[0];
}

void VulkanApp::createInstance()
{
	StackLog _(logStack, __FUNCTION__);
	/*std::cout << "    createInstance()" << std::endl;*/
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions;
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensions.push_back("VK_KHR_win32_surface");
	std::vector<const char*> validationLayers;
	validationLayers.push_back("VK_LAYER_KHRONOS_validation");

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void VulkanApp::setupDebugMessenger()
{
	StackLog _(logStack, __FUNCTION__);
	//std::cout << "    setupDebugMessenger()" << std::endl;

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

void VulkanApp::createCommandPool()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "    createCommandPool()" << std::endl;
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = 0;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}


void VulkanApp::createCommandBuffer()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "    createCommandBuffer()" << std::endl;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command buffer.");
	}
}

void VulkanApp::createFence()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "    createFence()" << std::endl;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create fence.");
	}

}

void VulkanApp::mainLoop()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "  mainLoop()" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		drawFrame();
	}
}

void VulkanApp::drawFrame()
{
	StackLog _(logStack, __FUNCTION__);

	//std::cout << "  drawFrame()" << std::endl;
	VkResult result{ VK_SUCCESS };
	result = vkDeviceWaitIdle(device);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to wait device idle!");
	}

	vkResetFences(device, 1, &fence);
	uint32_t imageIndex;
	result = vkAcquireNextImageKHR(device, renderTarget->swapChain, UINT64_MAX, VK_NULL_HANDLE, fence, &imageIndex);
	if (result == VK_NOT_READY)
	{
		return;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to acquire next image!");
	}
	vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
	recordCommandBuffer(commandBuffer, imageIndex);


	/*Submit draw*/ {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		if (vkQueueSubmit(graphicQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		result = vkDeviceWaitIdle(device);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to wait device idle!");
		}
	}

	/*Submit present*/ {
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 0;
		presentInfo.pWaitSemaphores = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &renderTarget->swapChain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present image!");
		}
	}
	imageIndex = (imageIndex + 1) % 2;


}



VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

