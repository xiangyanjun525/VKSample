#include <vulkan/vulkan.h>
#include "../util/MatrixState3D.h"
#include "MyVulkanManager.h"
#include "../util/FileUtil.h"
#include "../util/HelpFunction.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include "ThreadTask.h"
#include "../util/FPSUtil.h"
#include "TriangleData.h"
#include <time.h>
#include "../legencyUtil/util.hpp"

//当前使用的GPU索引 若只有一个GPU此值只可能是0
#define USED_GPU_INDEX 0

//内部使用的设置Image布局的方法
void setImageLayout(VkCommandBuffer cmd, VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout old_image_layout,
	VkImageLayout new_image_layout)
{
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.srcAccessMask = 0;
	image_memory_barrier.dstAccessMask = 0;
	image_memory_barrier.oldLayout = old_image_layout;
	image_memory_barrier.newLayout = new_image_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange.aspectMask = aspectMask;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;
	if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask =
			VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}

//========================================以上为文件内部使用的辅助方法=========================
//===================================以下为类的实现============================================

//静态成员实现
struct WindowInfo MyVulkanManager::info;
bool MyVulkanManager::loopDrawFlag = true;
std::vector<const char*>  MyVulkanManager::instanceExtensionNames;
VkInstance MyVulkanManager::instance;
uint32_t MyVulkanManager::gpuCount;
std::vector<VkPhysicalDevice> MyVulkanManager::gpus;
uint32_t MyVulkanManager::queueFamilyCount;
std::vector<VkQueueFamilyProperties> MyVulkanManager::queueFamilyprops;
uint32_t MyVulkanManager::queueGraphicsFamilyIndex;
VkQueue MyVulkanManager::queueGraphics;
uint32_t MyVulkanManager::queuePresentFamilyIndex;
std::vector<const char*> MyVulkanManager::deviceExtensionNames;
VkDevice MyVulkanManager::device;
VkCommandPool MyVulkanManager::cmdPool;
VkCommandBuffer MyVulkanManager::cmdBuffer;
VkCommandBufferBeginInfo MyVulkanManager::cmd_buf_info;
VkCommandBuffer  MyVulkanManager::cmd_bufs[1];
VkSubmitInfo MyVulkanManager::submit_info[1];
uint32_t MyVulkanManager::screenWidth;
uint32_t MyVulkanManager::screenHeight;
VkSurfaceKHR MyVulkanManager::surface;
std::vector<VkFormat> MyVulkanManager::formats;
VkSurfaceCapabilitiesKHR MyVulkanManager::surfCapabilities;
uint32_t MyVulkanManager::presentModeCount;
std::vector<VkPresentModeKHR> MyVulkanManager::presentModes;
VkExtent2D MyVulkanManager::swapchainExtent;
VkSwapchainKHR MyVulkanManager::swapChain;
uint32_t MyVulkanManager::swapchainImageCount;
std::vector<VkImage> MyVulkanManager::swapchainImages;
std::vector<VkImageView> MyVulkanManager::swapchainImageViews;
VkFormat MyVulkanManager::depthFormat;
VkFormatProperties MyVulkanManager::depthFormatProps;
VkImage MyVulkanManager::depthImage;
VkPhysicalDeviceMemoryProperties MyVulkanManager::memoryroperties;
VkDeviceMemory MyVulkanManager::memDepth;
VkImageView MyVulkanManager::depthImageView;
VkSemaphore MyVulkanManager::imageAcquiredSemaphore;
uint32_t MyVulkanManager::currentBuffer;
VkRenderPass MyVulkanManager::renderPass;
VkClearValue MyVulkanManager::clear_values[2];
VkRenderPassBeginInfo MyVulkanManager::rp_begin;
VkFence MyVulkanManager::taskFinishFence;
VkPresentInfoKHR MyVulkanManager::present;

VkFramebuffer* MyVulkanManager::framebuffers;

ShaderQueueSuit_Common* MyVulkanManager::sqsCL;
DrawableObjectCommonLight* MyVulkanManager::triForDraw;
float MyVulkanManager::xAngle = 0;
float MyVulkanManager::yAngle = 0;
float MyVulkanManager::zAngle = 0;

//创建Vulkan实例的方法
void MyVulkanManager::init_vulkan_instance()
{
	//初始化所需扩展列表
	instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	VkApplicationInfo app_info = {};//构建应用信息结构体实例
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;  //自定义数据的指针
	app_info.pApplicationName = "HelloVulakn"; //应用的名称
	app_info.applicationVersion = 1; //应用的版本号
	app_info.pEngineName = "HelloVulkan";//应用的引擎名称
	app_info.engineVersion = 1;//应用的引擎版本号
	app_info.apiVersion = VK_API_VERSION_1_0;//使用的Vulkan图形应用程序API版本

	VkInstanceCreateInfo inst_info = {};//构建实例创建信息结构体实例
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//结构体的类型
	inst_info.pNext = NULL;//自定义数据的指针
	inst_info.flags = 0;//供将来使用的标志
	inst_info.pApplicationInfo = &app_info;//绑定应用信息结构体
	inst_info.enabledExtensionCount = instanceExtensionNames.size();
	inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();

	inst_info.enabledLayerCount = 0;//启动的层数量
	inst_info.ppEnabledLayerNames = NULL;//启动的层名称列表

	VkResult result;//存储运行结果的辅助变量

	//创建Vulkan实例
	result = vkCreateInstance(&inst_info, NULL, &instance);//创建实例
	if (result == VK_SUCCESS)
	{
		printf("Vulkan实例创建成功!\n");
	}
	else
	{
		printf("Vulkan实例创建失败!\n");
	}
}

//销毁vulkan实例的方法
void MyVulkanManager::destroy_vulkan_instance()
{
	vkDestroyInstance(instance, nullptr);
	printf("Vulkan实例销毁完毕!\n");
	destroy_window(info);
}

//获取硬件设备的方法（GPU）
void MyVulkanManager::enumerate_vulkan_phy_devices()
{
	gpuCount = 0; //存储物理设备数量的变量
	VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
	assert(result == VK_SUCCESS);
	printf("[Vulkan硬件设备数量为%d个]", gpuCount);
	gpus.resize(gpuCount);
	VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());//填充物理设备列表
	assert(result == VK_SUCCESS);
	vkGetPhysicalDeviceMemoryProperties(gpus[USED_GPU_INDEX], &memoryroperties);//获取第一物理设备的内存属性
}

//创建逻辑设备的方法
void MyVulkanManager::create_vulkan_devices()
{
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, nullptr);//获取物理设备0中队列家族的数量
	printf("[Vulkan硬件设备0支持的队列家族数量为%d]\n", queueFamilyCount);
	queueFamilyprops.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, queueFamilyprops.data());//填充物理设备0队列家族属性列表
	printf("[成功获取Vulkan硬件设备0支持的队列家族属性列表]\n");

	VkDeviceQueueCreateInfo queueInfo = {};//构建设备队列创建信息结构体实例
	bool found = false; //辅助标志
	for (unsigned int i = 0; i < queueFamilyCount; i++) //遍历所有队列家族
	{
		if (queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) //若当前队列家族支持图形工作
		{
			queueInfo.queueFamilyIndex = i;//绑定此队列家族索引
			queueGraphicsFamilyIndex = i; //记录支持图形工作的队列家族索引
			printf("[支持GRAPHICS工作的一个队列家族的索引为%d]\n", i);
			printf("[此家族中的实际队列数量是%d]\n", queueFamilyprops[i].queueCount);
			found = true;
			break;
		}
	}

	float queue_priorities[1] = { 0.0 };
	float queue_priorities[1] = { 0.0 };//创建队列优先级数组
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;//给出结构体类型
	queueInfo.pNext = NULL;//自定义数据的指针
	queueInfo.queueCount = 1;//指定队列数量
	queueInfo.pQueuePriorities = queue_priorities;//给出每个队列的优先级
	queueInfo.queueFamilyIndex = queueGraphicsFamilyIndex;//绑定队列家族索引
	deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);//设置所需扩展

	VkDeviceCreateInfo deviceInfo = {};//构建逻辑设备创建信息结构体实例
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;//给出结构体类型
	deviceInfo.pNext = NULL;//自定义数据的指针
	deviceInfo.queueCreateInfoCount = 1;//指定设备队列创建信息结构体数量
	deviceInfo.pQueueCreateInfos = &queueInfo;//给定设备队列创建信息结构体列表
	deviceInfo.enabledExtensionCount = deviceExtensionNames.size();//所需扩展数量
	deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();//所需扩展列表
	deviceInfo.enabledLayerCount = 0;//需启动Layer的数量
	deviceInfo.ppEnabledLayerNames = NULL;//需启动Layer的名称列表
	deviceInfo.pEnabledFeatures = NULL;//启用的设备特性
	VkResult result = vkCreateDevice(gpus[0], &deviceInfo, NULL, &device);//创建逻辑设备
	assert(result == VK_SUCCESS);//检查逻辑设备是否创建成功
}

//销毁逻辑设备的方法
void MyVulkanManager::destroy_vulkan_devices()
{
	vkDestroyDevice(device, NULL);
	printf("逻辑设备销毁完毕！\n");
}

//创建命令缓冲的方法
void MyVulkanManager::create_vulkan_CommandBuffer()
{
	VkCommandPoolCreateInfo cmd_pool_info = {};
	VkCommandPoolCreateInfo cmd_pool_info = {};//构建命令池创建信息结构体实例
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 	//给定结构体类型
	cmd_pool_info.pNext = NULL;//自定义数据的指针
	cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;//绑定所需队列家族索引
	cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//执行控制标志
	VkResult result = vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmdPool);//创建命令池
	assert(result == VK_SUCCESS);//检查命令池创建是否成功

	VkCommandBufferAllocateInfo cmdBAI = {};
	VkCommandBufferAllocateInfo cmdBAI = {};//构建命令缓冲分配信息结构体实例
	cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;//给定结构体类型
	cmdBAI.pNext = NULL;//自定义数据的指针
	cmdBAI.commandPool = cmdPool;//指定命令池
	cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//分配的命令缓冲级别
	cmdBAI.commandBufferCount = 1;//分配的命令缓冲数量
	result = vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);//分配命令缓冲

	assert(result == VK_SUCCESS);//检查分配是否成功
	cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//给定结构体类型
	cmd_buf_info.pNext = NULL;//自定义数据的指针
	cmd_buf_info.flags = 0;//描述使用标志
	cmd_buf_info.pInheritanceInfo = NULL;//命令缓冲继承信息
	cmd_bufs[0] = cmdBuffer;//要提交到队列执行的命令缓冲数组

	VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();
	*pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();//目标管线阶段
	*pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info[0].pNext = NULL;//自定义数据的指针
	submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;//给定结构体类型
	submit_info[0].pWaitDstStageMask = pipe_stage_flags;//给定目标管线阶段
	submit_info[0].commandBufferCount = 1;//命令缓冲数量
	submit_info[0].pCommandBuffers = cmd_bufs;//提交的命令缓冲数组
	submit_info[0].signalSemaphoreCount = 0;//信号量数量
	submit_info[0].pSignalSemaphores = NULL;//信号量数组
}

void MyVulkanManager::destroy_vulkan_CommandBuffer()//销毁命令缓冲的方法
{

}