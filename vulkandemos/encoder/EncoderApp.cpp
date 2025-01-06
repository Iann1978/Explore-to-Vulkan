#include "EncoderApp.h"

// rgba2yuv420.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(lib, "vulkan-1.lib")

#define STB_IMAGE_WRITE_IMPLEMENTATION  

#include "stb_image_write.h" 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


EncoderApp::EncoderApp()
{
	std::cout << "EncoderApp()" << std::endl;
}

EncoderApp::~EncoderApp()
{
	std::cout << "~EncoderApp()" << std::endl;
}

void EncoderApp::run()
{
	std::cout << "run()" << std::endl;
	initVulkan();
	mainLoop();
	cleanup();
}


void EncoderApp::createEncodeSession()
{


	static const VkExtensionProperties h264EncodeStdExtensionVersion = { VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_SPEC_VERSION };

	// 1. Create the video session create info  
	VkVideoSessionCreateInfoKHR videoSessionCreateInfo;
	videoSessionCreateInfo.sType = VK_STRUCTURE_TYPE_VIDEO_SESSION_CREATE_INFO_KHR;
	videoSessionCreateInfo.pNext = nullptr;
	videoSessionCreateInfo.flags = 4;
	videoSessionCreateInfo.queueFamilyIndex = 4; // The queue family index that supports video operations
	videoSessionCreateInfo.pictureFormat = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM; // The picture format that will be used for the video operations
	videoSessionCreateInfo.maxCodedExtent = { 960, 400 }; // The maximum coded extent that will be used for the video operations
	videoSessionCreateInfo.maxDpbSlots = 5; // The maximum number of decoded picture buffer slots that will be used for the video operations
	videoSessionCreateInfo.maxActiveReferencePictures = 16;
	videoSessionCreateInfo.referencePictureFormat = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
	videoSessionCreateInfo.pStdHeaderVersion = &h264EncodeStdExtensionVersion;

	VkVideoProfileInfoKHR videoProfileInfo;
	videoProfileInfo.sType = VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
	videoProfileInfo.pNext = nullptr;
	videoProfileInfo.videoCodecOperation = VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR;
	videoProfileInfo.chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR;
	videoProfileInfo.lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;
	videoProfileInfo.chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;
	videoSessionCreateInfo.pVideoProfile = &videoProfileInfo;


	// 2. Create the video session
	PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR"));
	VkResult result = vkCreateVideoSessionKHR(device, &videoSessionCreateInfo, nullptr, &videoSession);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create video session");
	}
	

}


void EncoderApp::initVulkan()
{
	std::cout << "initVulkan()" << std::endl;
	showVulkanVersion();
	enumSupportedValidationLayer();
	createInstance();
	enumPhysicalDevices(instance);
	setupDebugMessenger();
	enumInstanceExtension();
	pickPhysicalDevice();
	enumSupportedPipeline(physicalDevice);
	enumSupportedQueueFamily(physicalDevice);
	checkVideoEncodeSupport(physicalDevice);
	//createLogicDevice();
	device = createLogicDevice(4);
	encodeQueue = pickEncodeQueue(physicalDevice);

	createEncodeSession();





}

void EncoderApp::drawFrame()
{

}


void EncoderApp::mainLoop()
{
	std::cout << "mainLoop()" << std::endl;
	while (true) {
		//glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(device);
}

void EncoderApp::cleanup()
{
	std::cout << "cleanup()" << std::endl;
}


bool EncoderApp::checkVideoEncodeSupport(VkPhysicalDevice physicalDevice)
{
	// 1. Get the list of available extensions  
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

	// 2. Check if the required extensions are present  
	bool supportsVideoQueue = false;
	bool supportsVideoEncodeQueue = false;

	for (const auto& extension : extensions) {
		if (strcmp(extension.extensionName, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME) == 0) {
			supportsVideoQueue = true;
		}
		if (strcmp(extension.extensionName, VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME) == 0) {
			supportsVideoEncodeQueue = true;
		}

		//VK_KHR_video_encode_h264
	}

	// 3. Check if both extensions are supported  
	return supportsVideoQueue && supportsVideoEncodeQueue;
}
