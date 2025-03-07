#pragma once

#include "include.h"

// should we create windows  in render target?
class RenderTarget
{
public:
	GLFWwindow* window;
	VkInstance instance{ VK_NULL_HANDLE };
	VkDevice device{ VK_NULL_HANDLE };
	VkSurfaceKHR surface{ VK_NULL_HANDLE };
	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
	VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkQueue graphicsQueue{ VK_NULL_HANDLE };
	VkQueue presentQueue{ VK_NULL_HANDLE };
	VkRenderPass renderPass{ VK_NULL_HANDLE };

public:
	RenderTarget(GLFWwindow* window, VkInstance instance, VkDevice device)
		: window(window), instance(instance), device(device) {
	};

	VkSurfaceKHR createSurface();
	void destroySurface(VkSurfaceKHR surface);
	VkSwapchainKHR createSwapChain();
	void destroySwapChain(VkSwapchainKHR swapChain);
	void getSwapChainImages();
	VkImageView createImageView(VkImage image/*, VkFormat format, VkImageAspectFlags aspectFlags*/);
	VkFramebuffer createFrameBuffer(VkImageView imageView, VkExtent2D extent, VkRenderPass renderPass);
	void createFramebuffers();

};

class Shader {
public:
	Shader(VkDevice device, VkRenderPass renderPass, const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	VkPipeline getPipeline() { return graphicsPipeline; }
	VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
	VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }

	VkDevice device;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkRenderPass renderPass;

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	void createPipelineLayout();
	//void createDescriptorSetLayout();
	void createGraphicsPipeline(const std::string vertexShaderPath, const std::string fragmentShaderPath);
	//void createDescriptorPool();

};