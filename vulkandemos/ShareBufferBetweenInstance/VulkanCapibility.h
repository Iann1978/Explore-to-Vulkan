#pragma once
#include "include.h"
#include "StackLog.h"

class VulkanCapibility
{
public:
	//VulkanCapibility(int& logStack) : logStack(logStack) {}
	//~VulkanCapibility();
public:
	void showVulkanVersion();
	void enumInstanceExternsions();
	void enumDeviceExtensions(VkPhysicalDevice physicalDevice);

private:
	//int& logStack;
};

