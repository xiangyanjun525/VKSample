#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vector>
#include <atomic>
#include "../util/DrawableObjectCommon.h"
#include "ShaderQueueSuit_Common.h"

#define FENCE_TIMEOUT 100000000

class MyVulkanManager
{
public:
	//窗口辅助接头体
	static struct WindowInfo info;
	//vulkan绘制的循环标志
	static std::atomic<bool> loopDrawFlag;
	static std::vector<const char*> instanceExtensionNames; //需要使用的实例扩展名称列表
	static VkInstance instance; //Vulkan实例
	static uint32_t gpuCount; //物理设备列表
	static std::vector<VkPhysicalDevice> gpus;  //物理设备列表
	static uint32_t queueFamilyCount; //物理设备对应的队里家族数量
	static std::vector<VkQueueFamilyProperties> queueFamilyprops; //物理设备对应的队列家族属性列表
	static uint32_t queueGraphicsFamilyIndex; //支持图形工作的队列家族索引
	static VkQueue queueGraphics; //支持图形工作的队列
	static uint32_t queuePresentFamilyIndex; //支持显示工作的队列家族索引
	static std::vector<const char*> deviceExtensionNames; //所需的设备扩展名称列表
	static VkDevice device; //逻辑设备
	static VkCommandPool cmdPool;	//命令池
	static VkCommandBuffer cmdBuffer; //命令缓冲
	static VkCommandBufferBeginInfo cmd_buf_info; //命令启动信息
	static VkCommandBuffer cmd_bufs[1]; //提供执行的命令缓冲数组
	static VkSubmitInfo submit_info[1]; //命令缓冲提交执行信息数组
	static uint32_t screenWidth;  //屏幕宽度
	static uint32_t screenHeight; //屏幕高度
	static VkSurfaceKHR surface; //KHR表面
	static std::vector<VkFormat> formats; //KHR表面支持的格式
	static VkSurfaceCapabilitiesKHR surfCapabilities; //表面的能力 
	static uint32_t presentModeCount; //显示模式数量
	static std::vector<VkPresentModeKHR> presentModes; //显示模式列表
	static VkExtent2D swapchainExtent; //交换链尺寸
	static VkSwapchainKHR swapChain; //交换链
	static uint32_t swapchainImageCount; //交换链中的图像数量
	static std::vector<VkImage> swapchainImages; //交换链中的图像列表
	static std::vector<VkImageView> swapchainImageViews; //交换链对应的图像视图列表
	static VkFormat depthFormat; //深度图像格式
	static VkFormatProperties depthFormatProps; //物理设备支持的深度格式属性
	static VkImage depthImage; //深度缓冲图像
	static VkPhysicalDeviceMemoryProperties memoryroperties; //物理设备内存属性
	static VkDeviceMemory memDepth;   //深度缓冲图像对应的内存
	static VkImageView depthImageView; //深度缓冲图像视图
	static VkSemaphore imageAcquiredSeampgore; //渲染目标图像获取完成信号量
	static uint32_t currentBuffer; //从交换链中获取的当前渲染用图像对应的编号缓冲
	static VkRenderPass renderPass; //渲染通道
	static VkClearValue clear_values[2]; //渲染通道用清除帧缓冲深度，颜色附件的数据
	static VkRenderPassBeginInfo rp_begin; //渲染通道启动信息
	static VkFence taskFinishFence;  //等待任务完成围栏
	static VkPresentInfoKHR present; //呈现信息
	static VkFramebuffer* framebuffers; //帧缓冲序列收指针
	static ShaderQueueSuit_Common* sqsCL; //着色器管线指针
	static DrawableObjectCommonLight* triForDraw; //绘制用三色三角形物体对象指针
	//三角形旋转角度
	static float xAngle;
	static float yAngle;
	static float zAngle;


};