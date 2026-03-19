#pragma once

#include <vulkan/vulkan.h>
#include <string>

class DrawableObjectCommonLight
{
public:
	VkDevice* devicePointer; //指向逻辑设备的指针
	float* vdata; //顶点数组首地址指针
	int vCount; //顶点数量
	VkBuffer vertexDatabuf; //顶点数据缓冲
	VkDeviceMemory vertexDataMem; //顶点数据缓冲描述信息
	VkDescriptorBufferInfo vertexDataBufferInfo; //顶点数据缓冲描述信息
	DrawableObjectCommonLight(float* vdataIn, int dataByteCount, int vCountIn,VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties);
	~DrawableObjectCommonLight();
	void drawSelf(VkCommandBuffer& secondary_cmd, VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkDescriptorSet* desSetPointer); //绘制方法
};