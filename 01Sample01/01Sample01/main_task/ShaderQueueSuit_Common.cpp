#include "ShaderQueueSuit_Common.h"
#include <assert.h>
#include "../util/HelpFunction.h"
#include "../util/FileUtil.h"
#include "MyVulkanManager.h"
#include "ShaderCompileUtil.h"
#include "../legencyUtil/util.hpp"
#include "PathData.h"

//创建Unifirm Buffer
void ShaderQueueSuit_Common::create_uniform_buffer(VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties)
{
	bufferByteCount = sizeof(float) * 16; //一致变量缓冲的总字节数

	VkBufferCreateInfo buf_info = {};//构建一致变量缓冲创建信息结构体实例
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;	//结构体的类型
	buf_info.pNext = nullptr; //自定义数据的指针
	buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; //缓冲的用途
	buf_info.size = bufferByteCount; //缓冲总字节数
	buf_info.queueFamilyIndexCount = 0; //队列家族数量
	buf_info.pQueueFamilyIndices = nullptr; //队列家族索引列表
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //共享模式
	buf_info.flags = 0; //标志

	VkResult result = vkCreateBuffer(device, &buf_info, nullptr, &uniformBuf); //创建一致变量缓冲
	assert(result == VK_SUCCESS);//检查创建是否成功

	VkMemoryRequirements mem_reqs; //内存需求变量
	vkGetBufferMemoryRequirements(device, uniformBuf, &mem_reqs); //获取此缓冲的内存需求

	VkMemoryAllocateInfo alloc_info = {};//构建内存分配信息结构体实例
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//结构体类型
	alloc_info.pNext = nullptr; //自定义数据的指针
	alloc_info.memoryTypeIndex = 0;//内存类型索引
	alloc_info.allocationSize = mem_reqs.size;//缓冲内存分配字节数

	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//需要的内存类型掩码
	bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);	//获取所需内存类型索引
	if (flag) { printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex); }
	else { printf("确定内存类型失败!"); }

	result = vkAllocateMemory(device, &alloc_info, NULL, &memUniformBuf);//分配内存
	assert(result == VK_SUCCESS);//检查内存分配是否成功
	result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);//将内存和对应缓冲绑定
	assert(result == VK_SUCCESS);//检查绑定操作是否成功

	uniformBufferInfo.buffer = uniformBuf;//指定一致变量缓冲
	uniformBufferInfo.offset = 0;//起始偏移量
	uniformBufferInfo.range = bufferByteCount;//一致变量缓冲总字节数
}

void ShaderQueueSuit_Common::destroy_uniform_buffer(VkDevice& device)//销毁一致变量缓冲相关
{
	vkDestroyBuffer(device, uniformBuf, NULL);//销毁一致变量缓冲
	vkFreeMemory(device, memUniformBuf, NULL);//释放一致变量缓冲对应设备内存
}

//创建管线layout
void ShaderQueueSuit_Common::create_pipeline_layout(VkDevice& device)
{

}