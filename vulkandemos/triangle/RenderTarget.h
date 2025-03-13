#pragma once

#include "include.h"

class StackLog
{
public:
	StackLog(int& logStack, std::string logstr)
		: logStack(logStack)
	{
		for (int i = 0; i < logStack; i++)
		{
			std::cout << "  ";
		}
		std::cout << logstr << std::endl;
		logStack++;
	}
	~StackLog() {
		logStack--;
	}
	int& logStack;
};

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
	int& logStack;
public:
	RenderTarget(GLFWwindow* window, VkInstance instance, VkDevice device, int& logStack);


	VkSurfaceKHR createSurface();
	void destroySurface(VkSurfaceKHR surface);
	VkSwapchainKHR createSwapChain();
	void destroySwapChain(VkSwapchainKHR swapChain);
	void getSwapChainImages();
	VkImageView createImageView(VkImage image/*, VkFormat format, VkImageAspectFlags aspectFlags*/);
	void createImageViews();
	VkFramebuffer createFrameBuffer(VkImageView imageView, VkExtent2D extent, VkRenderPass renderPass);
	void createFramebuffers();
	VkRenderPass createRenderPass();

};
class Mesh;
class Shader {
public:
	Shader(VkDevice device, VkRenderPass renderPass, const char* vertexShaderPath, const char* fragmentShaderPath, int& logStack);
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
	int& logStack;

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	void createPipelineLayout();
	void createDescriptorSetLayout();
	void createGraphicsPipeline(const std::string vertexShaderPath, const std::string fragmentShaderPath, Mesh* mesh);
	//void createDescriptorPool();

};

class Material {
	Shader* shader;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkDescriptorPool descriptorPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	int& logStack;

	std::map<std::string, glm::vec3> vec3Values;

	struct Ubo
	{
		VkBuffer buffer;
		VkDeviceMemory memory;
		void* data;
	} ubo;
public:
	Material(VkPhysicalDevice physicalDevice, VkDevice device,  Shader* shader, int& logStack);
	void SetVec3(std::string name, glm::vec3 value) { vec3Values[name] = value; }


	void Bind(VkCommandBuffer commandBuffer);
	void updateUbo();
private:
	void createUbo();

	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSet();


};

class Mesh {
public:
	Mesh(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, int& logStack);
	//~Mesh();
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	//VkBuffer getIndexBuffer() { return indexBuffer; }
	uint32_t getIndexCount() { return indexCount; }
	void createVertexBuffer(const std::vector<glm::vec3>& vertices);
	

	VkVertexInputBindingDescription  getVertexBindDestription();
	VkVertexInputAttributeDescription getAttributeDescription();
	//void createIndexBuffer(const std::vector<uint32_t>& indices);
	//void createUniformBuffer();
	//void updateUniformBuffer(glm::mat4 model, glm::mat4 view, glm::mat4 proj);
	//void Bind(VkCommandBuffer commandBuffer);
private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
	uint32_t indexCount;
	int& logStack;
};
		