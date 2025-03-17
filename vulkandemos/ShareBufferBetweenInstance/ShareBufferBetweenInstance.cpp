// ShareBufferBetweenInstance.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "VulkanCapibility.h"
#include "VulkanContext.h"

struct VulkanExtFuns {
	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = nullptr;
};

void InitVulkanExtFuns(VulkanExtFuns& extFuns, VulkanContext& context) {
    extFuns.vkGetMemoryWin32HandleKHR =
        (PFN_vkGetMemoryWin32HandleKHR)vkGetDeviceProcAddr(context.device, "vkGetMemoryWin32HandleKHR");
}
static uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    //StackLog _(logStack, __FUNCTION__);

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && memoryProperties.memoryTypes[i].propertyFlags & properties)
        {
            return i;
        }
    }
    return -1;

}

class App1
{
public:
	App1() {
        StackLog _(context.logStack, __FUNCTION__);
		VulkanContextBuilder builder(context);
		builder.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		builder.addInstanceExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
		builder.addInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		builder.addValidationLayer("VK_LAYER_KHRONOS_validation");
		builder.addDeviceExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
		builder.addDeviceExtension("VK_KHR_external_memory_win32");
		builder.build();
		InitVulkanExtFuns(extFuns, context);
        createExportableBuffer();
	}

public:
    void createExportableBuffer()
    {
        StackLog _(context.logStack, __FUNCTION__);
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = 1024; // Size of the buffer  
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkExternalMemoryBufferCreateInfo externalBufferInfo{};
        externalBufferInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO;
        externalBufferInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        bufferInfo.pNext = &externalBufferInfo;

        if (vkCreateBuffer(context.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(context.device, buffer, &memRequirements);

        VkExportMemoryAllocateInfo exportMemoryInfo{};
        exportMemoryInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        exportMemoryInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryTypeIndex(context.physicalDevice, memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); 
        allocInfo.pNext = &exportMemoryInfo;

        if (vkAllocateMemory(context.device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }

        vkBindBufferMemory(context.device, buffer, memory, 0);

        //PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR =
        //    (PFN_vkGetMemoryWin32HandleKHR)vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleKHR");

        // Export the memory handle  
        VkMemoryGetWin32HandleInfoKHR getHandleInfo{};
        getHandleInfo.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR;
        getHandleInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        getHandleInfo.memory = memory;

        if (extFuns.vkGetMemoryWin32HandleKHR(context.device, &getHandleInfo, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to get memory handle!");
        }

    }

    void write()
    {
        StackLog _(context.logStack, __FUNCTION__);
        float data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        void* mappedData;

        // Map the memory  
        vkMapMemory(context.device, memory, 0, sizeof(data), 0, &mappedData);
        memcpy(mappedData, data, sizeof(data));
        vkUnmapMemory(context.device, memory);
    }
public:
    VulkanCapibility capibility;
    VulkanContext context;
	VulkanExtFuns extFuns;
    VkBuffer buffer;
    VkDeviceMemory memory;
    HANDLE handle;

};

class App2
{
public:
    App2(HANDLE handle) 
		: handle(handle)
    {
        StackLog _(context.logStack, __FUNCTION__);
        VulkanContextBuilder builder(context);
        builder.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        builder.addInstanceExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
        builder.addInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        builder.addValidationLayer("VK_LAYER_KHRONOS_validation");
        builder.addDeviceExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
        builder.addDeviceExtension("VK_KHR_external_memory_win32");
        builder.build();
        InitVulkanExtFuns(extFuns, context);
        importBuffer();
    }

public:
    void importBuffer() {
        StackLog _(context.logStack, __FUNCTION__);
        // Import the memory handle  
        VkImportMemoryWin32HandleInfoKHR importInfo{};
        importInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
        importInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        importInfo.handle = handle;
        
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = 1024; // Buffer size in bytes  
        allocInfo.memoryTypeIndex = 1;
        allocInfo.pNext = &importInfo;
        
        if (vkAllocateMemory(context.device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory!");
        }
        


        // Create a buffer and bind the imported memory  
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = 1024;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT ;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkExternalMemoryBufferCreateInfo externalBufferInfo{};
        externalBufferInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO;
        externalBufferInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        bufferInfo.pNext = &externalBufferInfo;
        
        if (vkCreateBuffer(context.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer!");
        }
        
        vkBindBufferMemory(context.device, buffer, memory, 0);
    }

    void read()
    {
        StackLog _(context.logStack, __FUNCTION__);
        float readData[10];
        void* readMappedData;

        // Map the memory in the second instance  
        vkMapMemory(context.device, memory, 0, sizeof(readData), 0, &readMappedData);
        memcpy(readData, readMappedData, sizeof(readData));
        vkUnmapMemory(context.device, memory);

        // Output the read data  
        for (int i = 0; i < 10; ++i) {
            std::cout << readData[i] << " ";
        }
    }
public:
    VulkanCapibility capibility;
    VulkanContext context;
    VulkanExtFuns extFuns;
    VkBuffer buffer;
    VkDeviceMemory memory;
    HANDLE handle;
};

#include <thread>
#include <chrono>
int main()
{
    std::cout << "Hello World(ShareBufferBetweenInstance)!\n";

    try {
        App1 app1;
        App2 app2(app1.handle);

        app1.write();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app2.read();

		
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
