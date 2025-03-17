#pragma once
#include "include.h"

#include "include.h"

class VulkanContext
{
public:
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkDebugUtilsMessengerEXT debugMessenger;
	int logStack;
};


class VulkanContextBuilder
{
public:
	VulkanContextBuilder(VulkanContext& context) : context(context) {}
public:
	void addInstanceExtension(const char* extension)
	{
		instanceExtensions.push_back(extension);
	}
	void addValidationLayer(const char* layer)
	{
		validationLayers.push_back(layer);
	}
	void addDeviceExtension(const char* extension)
	{
		deviceExtensions.push_back(extension);
	}

public:
	void build();

private:
	void createInstance();
	void setupDebugMessenger();
	void createDevice();
	VulkanContext& context;
	//void createDevice(VkInstance instance, VkDevice& device, VkPhysicalDevice& physicalDevice);

private:
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;

};