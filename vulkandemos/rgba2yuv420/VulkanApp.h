#pragma once


#include "VulkanAppBase.h"


class VulkanApp : public VulkanAppBase
{



	VkQueue computeQueue{ VK_NULL_HANDLE };
	VkDevice device{ VK_NULL_HANDLE };
	VkBuffer stagingBuffer{ VK_NULL_HANDLE };
	VkDeviceMemory stagingBufferMemory{ VK_NULL_HANDLE };
	VkImage rgbaImage{ VK_NULL_HANDLE };
	VkDeviceMemory rgbaImageMemory{ VK_NULL_HANDLE };
	VkImageView rgbaImageView{ VK_NULL_HANDLE };
	VkImage yuvImage{ VK_NULL_HANDLE };
	VkDeviceMemory yuvImageMemory{ VK_NULL_HANDLE };
	VkImageView yuvImageView{ VK_NULL_HANDLE };
	VkCommandPool commandPool{ VK_NULL_HANDLE };
	VkCommandBuffer uploadRGBAImageCommandBuffer{ VK_NULL_HANDLE };
	VkCommandBuffer copyFromRGBAImageToYUVImageCommandBuffer{ VK_NULL_HANDLE };
	VkCommandBuffer downloadYUVImageCommandBuffer{ VK_NULL_HANDLE };
	VkCommandBuffer computeCommandBuffer{ VK_NULL_HANDLE };
	VkDescriptorSetLayout descriptorSetLayout { VK_NULL_HANDLE };
	VkPipeline computePipeline{ VK_NULL_HANDLE };
	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
	VkDescriptorPool descriptorPool { VK_NULL_HANDLE };
	VkDescriptorSet descriptorSet { VK_NULL_HANDLE };
	int width = 512;
	int height = 512;
	const char* src_img_path = "texture.jpg";
	const char* dst_img_path = "texture_yuv.jpg";

public:
	VulkanApp();

	~VulkanApp();

	void run();

private:

	//void createInstance();














	void createLogicDevice();

	void pickComputeQueue();

	void createRGBAImage();


	void createYUVImage();





	void createCommandPool();


	VkPipeline createComputePipeline1(VkDevice device, VkShaderModule shaderModule, VkPipelineLayout pipelineLayout);

	VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);

	void updateDescriptorSets(VkDevice device, VkDescriptorSet descriptorSet, VkImageView srcImageView, VkImageView dstImageView);

	void dispatchComputeShader(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet, uint32_t width, uint32_t height);

	VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);


	void createComputePipeline();

	void createUploadRGBACommandBuffer();


	void createCopyFromRGBAImageToYUVImageCommandBuffer();

	void createDownloadYUVImageCommandBuffer();




	void createComputeCommandBuffer();

	void recordCommandBuffer(VkCommandBuffer commandBuffer);

	void createStagingBuffer();

	void loadImageToStagingBuffer();

	void saveImageFromStagingBuffer();

	void recordUploadRGBAImageCommandBuffer();

	void recordCopyFromRGBAImageToYUVImageCommandBuffer();

	void recordDownloadYUVImageCommandBuffer();

	



	void createRgba2YuvPipiline();


	void initVulkan();

	void drawFrame();

	void mainLoop();

	void cleanup();


};
