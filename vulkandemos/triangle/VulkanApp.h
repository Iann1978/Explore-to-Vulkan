#pragma once

#include "RenderTarget.h"

//class RenderTarget
//{
//public:
//	GLFWwindow* window;
//	VkInstance instance{ VK_NULL_HANDLE };
//	VkDevice device{ VK_NULL_HANDLE };
//	VkSurfaceKHR surface{ VK_NULL_HANDLE };
//	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
//	VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
//	VkFormat swapChainImageFormat;
//	VkExtent2D swapChainExtent;
//	std::vector<VkImage> swapChainImages;
//	std::vector<VkImageView> swapChainImageViews;
//	VkQueue graphicsQueue{ VK_NULL_HANDLE };
//	VkQueue presentQueue{ VK_NULL_HANDLE };
//	VkRenderPass renderPass{ VK_NULL_HANDLE };
//
//public:
//	RenderTarget(GLFWwindow* window, VkInstance instance, VkDevice device)
//		: window(window), instance(instance), device(device) {
//	};
//
//	VkSurfaceKHR createSurface(GLFWwindow* window);
//	void destroySurface(VkSurfaceKHR surface);
//	VkSwapchainKHR createSwapChain();
//	void destroySwapChain(VkSwapchainKHR swapChain);
//	void getSwapChainImages();
//
//
//
//
//};
class VulkanApp
{
public:
public:
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicQueue;
	VkQueue presentQueue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	RenderTarget *renderTarget = nullptr;
	Shader* shader = nullptr;
	Material* material = nullptr;
	Mesh* mesh = nullptr;
	VkFence fence;
	int logStack = 0;

public:

	void enumSupportedValidationLayer();



	void run();
	void initWindow();
	void initVulkan();
	void createInstance();
	void setupDebugMessenger();
	VkPhysicalDevice pickPhysicalDevice();
	VkDevice createLogicDevice();
	VkRenderPass createRenderPass();
	VkQueue pickGraphicQueue();
	VkQueue pickPresentQueue();
	void createCommandPool();
	void createCommandBuffer();
	void createFence();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	


	//void drawFrame();

	void mainLoop();
	void drawFrame();

	//void cleanup();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);


};
