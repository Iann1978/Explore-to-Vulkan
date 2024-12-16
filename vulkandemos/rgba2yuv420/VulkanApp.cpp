#include "VulkanApp.h"

// rgba2yuv420.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(lib, "vulkan-1.lib")

#define STB_IMAGE_WRITE_IMPLEMENTATION  

#include "stb_image_write.h" 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>








VulkanApp::VulkanApp()
{
	std::cout << "VulkanApp()" << std::endl;
}

VulkanApp::~VulkanApp()
{
	std::cout << "~VulkanApp()" << std::endl;
}

void VulkanApp::run()
{
	std::cout << "run()" << std::endl;
	initVulkan();
	mainLoop();
	cleanup();
}



void VulkanApp::pickPhysicalDevice()
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


void VulkanApp::createLogicDevice()
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

	auto queryIndex = findQueueFamilyIndex(physicalDevice, VK_QUEUE_COMPUTE_BIT);
	if (!queryIndex.has_value()) {
		throw std::runtime_error("failed to find compute queue!");
	}

	vkGetDeviceQueue(device, queryIndex.value(), 0, &computeQueue);

}

void VulkanApp::createRGBAImage()
{
	std::cout << "createRGBAImage()" << std::endl;
	VkResult result = VK_SUCCESS;


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

	result = vkCreateImage(device, &imageInfo, nullptr, &rgbaImage);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create rgba image!");
	}



	// Allocate memory
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, rgbaImage, &memRequirements);

	// find memory type index
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			memoryTypeIndex = i;
			break;
		}
	}


	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &allocInfo, nullptr, &rgbaImageMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate rgba image memory!");
	}

	result = vkBindImageMemory(device, rgbaImage, rgbaImageMemory, 0);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to bind rgba image memory!");
	}

	// Create image view
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = rgbaImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;


	if (vkCreateImageView(device, &viewInfo, nullptr, &rgbaImageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create rgba image views!");
	}






}



void VulkanApp::createYUVImage()
{
	std::cout << "createYUVImage()" << std::endl;
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = 512;
	imageInfo.extent.height = 512;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateImage(device, &imageInfo, nullptr, &yuvImage);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create yuv image!");
	}

	// Allocate memory
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, yuvImage, &memRequirements);

	// find memory type index
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			memoryTypeIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &allocInfo, nullptr, &yuvImageMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate yuv image memory!");
	}

	result = vkBindImageMemory(device, yuvImage, yuvImageMemory, 0);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to bind yuv image memory!");
	}

	// Create image view
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = yuvImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewInfo, nullptr, &yuvImageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create yuv image views!");
	}




}


void VulkanApp::createCommandPool()
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


VkPipeline VulkanApp::createComputePipeline1(VkDevice device, VkShaderModule shaderModule, VkPipelineLayout pipelineLayout) {
	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = shaderStageInfo;
	pipelineInfo.layout = pipelineLayout;

	VkPipeline pipeline;
	if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute pipeline!");
	}

	return pipeline;
}
VkDescriptorSetLayout VulkanApp::createDescriptorSetLayout(VkDevice device)
{
	std::cout << "createDescriptorSetLayout()" << std::endl;
	// Descriptor set layout binding for the source image  
	VkDescriptorSetLayoutBinding srcImageBinding = {};
	srcImageBinding.binding = 0;
	srcImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;// VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	srcImageBinding.descriptorCount = 1;
	srcImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	srcImageBinding.pImmutableSamplers = nullptr;

	// Descriptor set layout binding for the destination image  
	VkDescriptorSetLayoutBinding dstImageBinding = {};
	dstImageBinding.binding = 1;
	dstImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	dstImageBinding.descriptorCount = 1;
	dstImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	dstImageBinding.pImmutableSamplers = nullptr;

	// Create the descriptor set layout  
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	VkDescriptorSetLayoutBinding bindings[] = { srcImageBinding, dstImageBinding };
	layoutInfo.pBindings = bindings;

	VkDescriptorSetLayout descriptorSetLayout;
	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		// Handle error  
		return VK_NULL_HANDLE;
	}

	return descriptorSetLayout;
}

void VulkanApp::updateDescriptorSets(VkDevice device, VkDescriptorSet descriptorSet, VkImageView srcImageView, VkImageView dstImageView) {
	VkDescriptorImageInfo srcImageInfo = {};
	srcImageInfo.imageView = srcImageView;
	srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkDescriptorImageInfo dstImageInfo = {};
	dstImageInfo.imageView = dstImageView;
	dstImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet descriptorWrites[2] = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pImageInfo = &srcImageInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &dstImageInfo;

	vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
}

void VulkanApp::dispatchComputeShader(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet, uint32_t width, uint32_t height) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	uint32_t groupCountX = (width + 15) / 16; // Match local_size_x = 16  
	uint32_t groupCountY = (height + 15) / 16; // Match local_size_y = 16  

	vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
}

VkPipelineLayout VulkanApp::createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Number of descriptor set layouts  
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Pointer to the descriptor set layout  

	// Optionally, define push constant ranges  
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Stages that can use the push constants  
	pushConstantRange.offset = 0; // Offset in bytes  
	pushConstantRange.size = sizeof(float) * 4; // Size of the push constants  

	pipelineLayoutInfo.pushConstantRangeCount = 1; // Number of push constant ranges  
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Pointer to the push constant range  

	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	return pipelineLayout;
}
VkDescriptorPool createDescriptorPool(VkDevice device)
{
	// Specify the number and types of descriptor sets that can be allocated from the pool  
	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2; // Number of pool sizes  
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1; // Maximum number of descriptor sets that can be allocated from the pool  

	VkDescriptorPool descriptorPool;
	VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		// Handle error  
		return VK_NULL_HANDLE;
	}

	return descriptorPool;
}

VkDescriptorSet allocateDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	VkDescriptorSet descriptorSet;
	VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
	if (result != VK_SUCCESS)
	{
		// Handle error  
		return VK_NULL_HANDLE;
	}

	return descriptorSet;
}

void updateDescriptorSets(VkDevice device, VkDescriptorSet descriptorSet, VkImage srcImage, VkImage dstImage)
{
	// Create image info for the source image  
	VkImageViewCreateInfo srcImageViewInfo = {};
	srcImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	srcImageViewInfo.image = srcImage;
	srcImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	srcImageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	srcImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	srcImageViewInfo.subresourceRange.levelCount = 1;
	srcImageViewInfo.subresourceRange.layerCount = 1;

	VkImageView srcImageView;
	VkResult result = vkCreateImageView(device, &srcImageViewInfo, nullptr, &srcImageView);
	if (result != VK_SUCCESS)
	{
		// Handle error  
		return;
	}

	// Create image info for the destination image  
	VkImageViewCreateInfo dstImageViewInfo = {};
	dstImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	dstImageViewInfo.image = dstImage;
	dstImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	dstImageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	dstImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstImageViewInfo.subresourceRange.levelCount = 1;
	dstImageViewInfo.subresourceRange.layerCount = 1;

	VkImageView dstImageView;
	result = vkCreateImageView(device, &dstImageViewInfo, nullptr, &dstImageView);
	if (result != VK_SUCCESS)
	{
		// Handle error  
		vkDestroyImageView(device, srcImageView, nullptr);
		return;
	}

	// Update the descriptor set with the image views  
	VkDescriptorImageInfo srcImageInfo = {};
	srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	srcImageInfo.imageView = srcImageView;
	srcImageInfo.sampler = VK_NULL_HANDLE;

	VkDescriptorImageInfo dstImageInfo = {};
	dstImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	dstImageInfo.imageView = dstImageView;
	dstImageInfo.sampler = VK_NULL_HANDLE;

	VkWriteDescriptorSet writeDescriptorSets[2] = {};
	writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[0].dstSet = descriptorSet;
	writeDescriptorSets[0].dstBinding = 0;
	writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].pImageInfo = &srcImageInfo;

	writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[1].dstSet = descriptorSet;
	writeDescriptorSets[1].dstBinding = 1;
	writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSets[1].descriptorCount = 1;
	writeDescriptorSets[1].pImageInfo = &dstImageInfo;

	vkUpdateDescriptorSets(device, 2, writeDescriptorSets, 0, nullptr);

	// Clean up  
	vkDestroyImageView(device, srcImageView, nullptr);
	vkDestroyImageView(device, dstImageView, nullptr);
}

void VulkanApp::createComputePipeline() {
	std::cout << "createComputePipeline()" << std::endl;


	// 1. Create a compute shader  
	auto compShaderCode = loadShaderCode("shader/rgba2yuv420.comp.spv");
	VkShaderModule computeShaderModule = createShaderModule(device, compShaderCode);

	// 2. Create a compute pipeline layout  
	descriptorSetLayout = createDescriptorSetLayout(device);
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

	// 3. Create a compute pipeline  
	VkComputePipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	pipelineInfo.stage.module = computeShaderModule;
	pipelineInfo.stage.pName = "main";
	pipelineInfo.layout = pipelineLayout;
	//VkPipeline computePipeline;
	vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline);

	// 4. Create a descriptor pool and allocate a descriptor set  
	descriptorPool = createDescriptorPool(device);
	descriptorSet = allocateDescriptorSet(device, descriptorPool, descriptorSetLayout);

	// 5. Update the descriptor set with the source and destination images  
	updateDescriptorSets(device, descriptorSet, rgbaImageView, yuvImageView);

	//// 6. Begin a command buffer  
	//VkCommandBuffer commandBuffer; // Assume the command buffer is already created and begun  
	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	//// 7. Dispatch the compute shader  
	//uint32_t groupCountX = (width + 15) / 16;
	//uint32_t groupCountY = (height + 15) / 16;
	//uint32_t groupCountZ = 1;
	//vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

	//// 8. End the command buffer and submit it for execution  
	//vkEndCommandBuffer(commandBuffer);
	//// Submit the command buffer for execution  

	//// Clean up  
	//vkDestroyPipeline(device, computePipeline, nullptr);
	//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	//vkDestroyShaderModule(device, computeShaderModule, nullptr);
	//vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	//vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void VulkanApp::createUploadRGBACommandBuffer()
{
	std::cout << "createUploadRGBACommandBuffer()" << std::endl;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	//VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &uploadRGBAImageCommandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer!");
	}
}

void VulkanApp::createCopyFromRGBAImageToYUVImageCommandBuffer()
{
	std::cout << "createCopyFromRGBAImageToYUVImageCommandBuffer()" << std::endl;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	//VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &copyFromRGBAImageToYUVImageCommandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer!");
	}
}

void VulkanApp::createDownloadYUVImageCommandBuffer()
{
	std::cout << "createDownloadYUVImageCommandBuffer()" << std::endl;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	//VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &downloadYUVImageCommandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer!");
	}
}

void VulkanApp::createComputeCommandBuffer()
{
	std::cout << "createComputeCommandBuffer()" << std::endl;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	//VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &computeCommandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer!");
	}
}


void VulkanApp::recordCommandBuffer(VkCommandBuffer commandBuffer) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}


	// Transition rgba image layout
	VkImageMemoryBarrier rgbaImageBarrier = {};
	rgbaImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	rgbaImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	rgbaImageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	rgbaImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	rgbaImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	rgbaImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	rgbaImageBarrier.image = rgbaImage;
	rgbaImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	rgbaImageBarrier.subresourceRange.baseMipLevel = 0;
	rgbaImageBarrier.subresourceRange.levelCount = 1;
	rgbaImageBarrier.subresourceRange.baseArrayLayer = 0;
	rgbaImageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &rgbaImageBarrier
	);


	// Transition yuv image layout
	VkImageMemoryBarrier yuvImageBarrier = {};
	yuvImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	yuvImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	yuvImageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	yuvImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	yuvImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.image = yuvImage;
	yuvImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	yuvImageBarrier.subresourceRange.baseMipLevel = 0;
	yuvImageBarrier.subresourceRange.levelCount = 1;
	yuvImageBarrier.subresourceRange.baseArrayLayer = 0;
	yuvImageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &yuvImageBarrier
	);


	// Bind the compute pipeline  
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

	// Bind descriptor set  
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	// Dispatch the compute shader  
	vkCmdDispatch(commandBuffer, 32, 32, 1); // Adjust the group count as needed  

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}



void VulkanApp::createStagingBuffer()
{
	std::cout << "createStagingBuffer()" << std::endl;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = width * height * 4;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create staging buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			memoryTypeIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate staging buffer memory!");
	}

	result = vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to bind staging buffer memory!");
	}

}


void VulkanApp::loadImageToStagingBuffer()
{
	std::cout << "loadImageToStagingBuffer()" << std::endl;
	// load image from file src_img_path using stb_image and copy it to stagingBuffer
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(src_img_path, &texWidth, &texHeight, &texChannels, STBI_default);
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}





	void* data = nullptr;
	VkResult result = vkMapMemory(device, stagingBufferMemory, 0, width * height * 4, 0, &data);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to map memory!");
	}
	//memcpy(data, pixels, width * height * 4); // Copy pixel data  

	size_t pixelCount = width * height;
	uint8_t* rgbaData = (uint8_t*)data;
	for (size_t i = 0; i < pixelCount; ++i) {
		rgbaData[i * 4 + 0] = pixels[i * 3 + 0]; // Copy Red  
		rgbaData[i * 4 + 1] = pixels[i * 3 + 1]; // Copy Green  
		rgbaData[i * 4 + 2] = pixels[i * 3 + 2]; // Copy Blue  
		rgbaData[i * 4 + 3] = 255;               // Set Alpha to 255 (fully opaque)  
	}
	vkUnmapMemory(device, stagingBufferMemory);
}

void VulkanApp::saveImageFromStagingBuffer()
{
	std::cout << "saveImageFromStagingBuffer()" << std::endl;
	unsigned char* pixels = (unsigned char*)malloc(width * height * 3); // RGB format  
	void* data = nullptr;
	VkResult result = vkMapMemory(device, stagingBufferMemory, 0, width * height * 4, 0, &data);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to map memory!");
	}
	//memcpy(data, pixels, width * height * 4); // Copy pixel data  

	size_t pixelCount = width * height;
	uint8_t* rgbaData = (uint8_t*)data;
	for (size_t i = 0; i < pixelCount; ++i) {
		pixels[i * 3 + 0] = rgbaData[i * 4 + 0]; // Copy Red  
		pixels[i * 3 + 1] = rgbaData[i * 4 + 1]; // Copy Green  
		pixels[i * 3 + 2] = rgbaData[i * 4 + 2]; // Copy Blue  
	}

	vkUnmapMemory(device, stagingBufferMemory);

	// Save the image  
	stbi_write_png("output.png", width, height, 3, pixels, width * 3);

	// Clean up  
	free(pixels);
}

void VulkanApp::recordUploadRGBAImageCommandBuffer()
{
	std::cout << "recordUploadRGBAImageCommandBuffer()" << std::endl;
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(uploadRGBAImageCommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// Transition rgba image layout
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = rgbaImage;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(
		uploadRGBAImageCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrier
	);

	// Copy data from staging buffer to rgba image
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { uint32_t(width), uint32_t(height), 1 };

	vkCmdCopyBufferToImage(uploadRGBAImageCommandBuffer, stagingBuffer, rgbaImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	vkEndCommandBuffer(uploadRGBAImageCommandBuffer);

}

void VulkanApp::recordCopyFromRGBAImageToYUVImageCommandBuffer()
{
	std::cout << "recordCopyFromRGBAImageToYUVImageCommandBuffer()" << std::endl;
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(copyFromRGBAImageToYUVImageCommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// Transition rgba image layout
	VkImageMemoryBarrier rgbaImageBarrier = {};
	rgbaImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	rgbaImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	rgbaImageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	rgbaImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	rgbaImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	rgbaImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	rgbaImageBarrier.image = rgbaImage;
	rgbaImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	rgbaImageBarrier.subresourceRange.baseMipLevel = 0;
	rgbaImageBarrier.subresourceRange.levelCount = 1;
	rgbaImageBarrier.subresourceRange.baseArrayLayer = 0;
	rgbaImageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		copyFromRGBAImageToYUVImageCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &rgbaImageBarrier
	);

	// Transition yuv image layout
	VkImageMemoryBarrier yuvImageBarrier = {};
	yuvImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	yuvImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	yuvImageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	yuvImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	yuvImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.image = yuvImage;
	yuvImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	yuvImageBarrier.subresourceRange.baseMipLevel = 0;
	yuvImageBarrier.subresourceRange.levelCount = 1;
	yuvImageBarrier.subresourceRange.baseArrayLayer = 0;
	yuvImageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		copyFromRGBAImageToYUVImageCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &yuvImageBarrier
	);

	// Copy image data
	VkImageCopy copyRegion = {};
	copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.layerCount = 1;
	copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.layerCount = 1;
	copyRegion.extent.width = width;
	copyRegion.extent.height = height;
	copyRegion.extent.depth = 1;

	vkCmdCopyImage(
		copyFromRGBAImageToYUVImageCommandBuffer,
		rgbaImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		yuvImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copyRegion
	);

	vkEndCommandBuffer(copyFromRGBAImageToYUVImageCommandBuffer);

}


void VulkanApp::recordDownloadYUVImageCommandBuffer()
{
	std::cout << "recordDownloadYUVImageCommandBuffer()" << std::endl;
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(downloadYUVImageCommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// Transition yuv image layout
	VkImageMemoryBarrier yuvImageBarrier = {};
	yuvImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	yuvImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	yuvImageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	yuvImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	yuvImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	yuvImageBarrier.image = yuvImage;
	yuvImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	yuvImageBarrier.subresourceRange.baseMipLevel = 0;
	yuvImageBarrier.subresourceRange.levelCount = 1;
	yuvImageBarrier.subresourceRange.baseArrayLayer = 0;
	yuvImageBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		downloadYUVImageCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &yuvImageBarrier
	);

	// Copy data from yuv image to staging buffer
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { uint32_t(width), uint32_t(height), 1 };

	vkCmdCopyImageToBuffer(downloadYUVImageCommandBuffer, yuvImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

	vkEndCommandBuffer(downloadYUVImageCommandBuffer);
}




void VulkanApp::createRgba2YuvPipiline()
{
	auto computeShaderCode = loadShaderCode("shaders/rgba2yuv.comp.spv");

	VkShaderModule computeShaderModule = createShaderModule(device, computeShaderCode);

	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStageInfo.module = computeShaderModule;
	computeShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { computeShaderStageInfo };

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = computeShaderStageInfo;

}


void VulkanApp::initVulkan()
{
	std::cout << "initVulkan()" << std::endl;
	enumSupportedValidationLayer();
	createInstance();
	enumPhysicalDevices(instance);
	setupDebugMessenger();
	enumInstanceExtension();
	pickPhysicalDevice();
	enumSupportedPipeline(physicalDevice);
	enumSupportedQueueFamily(physicalDevice);
	createLogicDevice();
	createStagingBuffer();
	createRGBAImage();
	createYUVImage();
	createCommandPool();
	createUploadRGBACommandBuffer();
	createCopyFromRGBAImageToYUVImageCommandBuffer();
	createDownloadYUVImageCommandBuffer();
	createComputeCommandBuffer();
	createComputePipeline();
	//loadImageToStagingBuffer();
	//saveImageFromStagingBuffer();
	//recordUploadRGBAImageCommandBuffer();



}

void VulkanApp::drawFrame()
{
	loadImageToStagingBuffer();
	VkResult result;
	uint32_t imageIndex = 0;
	std::cout << "drawFrame()" << std::endl;
	result = vkDeviceWaitIdle(device);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to wait for device!");
	}

	vkResetCommandBuffer(uploadRGBAImageCommandBuffer, 0);
	vkResetCommandBuffer(copyFromRGBAImageToYUVImageCommandBuffer, 0);
	vkResetCommandBuffer(downloadYUVImageCommandBuffer, 0);
	vkResetCommandBuffer(computeCommandBuffer, 0);
	recordUploadRGBAImageCommandBuffer();
	recordCopyFromRGBAImageToYUVImageCommandBuffer();
	recordDownloadYUVImageCommandBuffer();
	recordCommandBuffer(computeCommandBuffer);

	/* submit upload */ {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &uploadRGBAImageCommandBuffer;
		submitInfo.signalSemaphoreCount = 0;

		if (vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		result = vkDeviceWaitIdle(device);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to wait for device!");

		}
	}

	/* submit compute */ {
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &computeCommandBuffer;

		// Submit the command buffer to the queue  
		VkResult result = vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
		if (result != VK_SUCCESS){
			throw std::runtime_error("failed to submit compute command buffer!");
		}
		result = vkDeviceWaitIdle(device);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to wait for device!");

		}
	}

	//dispatchComputeShader()

	if (0) {/* submit copy */
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyFromRGBAImageToYUVImageCommandBuffer;
		submitInfo.signalSemaphoreCount = 0;

		if (vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		result = vkDeviceWaitIdle(device);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to wait for device!");

		}
	}

	/* submit download */ {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &downloadYUVImageCommandBuffer;
		submitInfo.signalSemaphoreCount = 0;

		if (vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		result = vkDeviceWaitIdle(device);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to wait for device!");
		}
	}

	saveImageFromStagingBuffer();
}


void VulkanApp::mainLoop()
{
	std::cout << "mainLoop()" << std::endl;
	while (true) {
		//glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(device);
}

void VulkanApp::cleanup()
{
	std::cout << "cleanup()" << std::endl;
}

