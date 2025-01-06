#pragma once


#include "VulkanAppBase.h"


class EncoderApp : public VulkanAppBase
{
	VkQueue encodeQueue{ VK_NULL_HANDLE };
	VkVideoSessionKHR videoSession{ VK_NULL_HANDLE };
public:
	bool checkVideoEncodeSupport(VkPhysicalDevice physicalDevice);

	//void createVideoSession(VkPhysicalDevice physicalDevice, VkDevice device, VkVideoSessionKHR* videoSession);

public:
	EncoderApp();

	~EncoderApp();


	void createEncodeSession();


	void initVulkan();

	void run();

	void drawFrame();

	void mainLoop();

	void cleanup();


};
