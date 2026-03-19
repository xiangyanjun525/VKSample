#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <filesystem>
#include "util.hpp"
#include "SPIRV/GlslangToSpv.h"
#include <Windows.h>
#include "../main_task/MyVulkanManager.h"

// 窗口初始化
void init_window(struct WindowInfo& info, int32_t default_width, int32_t default_height)
{
    // 简单的窗口创建，实际可能需要更多设置
    const char* CLASS_NAME = "VulkanWindowClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    info.width = default_width;
    info.height = default_height;
    strncpy_s(info.name, APP_NAME_STR_LEN, "Vulkan Sample", APP_NAME_STR_LEN - 1);

    info.window = CreateWindowExA(
        0,
        CLASS_NAME,
        info.name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        default_width, default_height,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );
}

// 窗口销毁
void destroy_window(struct WindowInfo& info)
{
    if (info.window)
    {
        DestroyWindow(info.window);
        info.window = NULL;
    }
}

// 初始化glslang
void init_glslang()
{
    glslang::InitializeProcess();
}

// 清理glslang
void finalize_glslang()
{
    glslang::FinalizeProcess();
}

// 等待指定秒数
void wait_seconds(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// 打印UUID
void print_UUID(uint8_t* pipelineCacheUUID)
{
    for (int i = 0; i < VK_UUID_SIZE; ++i)
    {
        printf("%02x", pipelineCacheUUID[i]);
    }
    printf("\n");
}

// 获取文件目录
std::string get_file_directory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path = buffer;
    size_t pos = path.find_last_of("\\/");
    return (pos != std::string::npos) ? path.substr(0, pos) : "";
}

// 获取毫秒时间戳
timestamp_t get_milliseconds()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

// 初始化glslang资源
void init_resources(TBuiltInResource& Resources)
{
    // 默认资源限制，根据实际需要调整
    Resources.maxLights = 32;
    Resources.maxClipPlanes = 6;
    Resources.maxTextureUnits = 32;
    Resources.maxTextureCoords = 32;
    Resources.maxVertexAttribs = 64;
    Resources.maxVertexUniformComponents = 4096;
    Resources.maxVaryingFloats = 64;
    Resources.maxVertexTextureImageUnits = 32;
    Resources.maxCombinedTextureImageUnits = 80;
    Resources.maxTextureImageUnits = 32;
    Resources.maxFragmentUniformComponents = 4096;
    Resources.maxDrawBuffers = 32;
    Resources.maxVertexUniformVectors = 128;
    Resources.maxVaryingVectors = 8;
    Resources.maxFragmentUniformVectors = 16;
    Resources.maxVertexOutputVectors = 16;
    Resources.maxFragmentInputVectors = 15;
    Resources.minProgramTexelOffset = -8;
    Resources.maxProgramTexelOffset = 7;
    Resources.maxClipDistances = 8;
    Resources.maxComputeWorkGroupCountX = 65535;
    Resources.maxComputeWorkGroupCountY = 65535;
    Resources.maxComputeWorkGroupCountZ = 65535;
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 64;
    Resources.maxComputeUniformComponents = 1024;
    Resources.maxComputeTextureImageUnits = 16;
    Resources.maxComputeImageUniforms = 8;
    Resources.maxComputeAtomicCounters = 8;
    Resources.maxComputeAtomicCounterBuffers = 1;
    Resources.maxVaryingComponents = 60;
    Resources.maxVertexOutputComponents = 64;
    Resources.maxGeometryInputComponents = 64;
    Resources.maxGeometryOutputComponents = 128;
    Resources.maxFragmentInputComponents = 128;
    Resources.maxImageUnits = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    Resources.maxCombinedShaderOutputResources = 8;
    Resources.maxImageSamples = 0;
    Resources.maxVertexImageUniforms = 0;
    Resources.maxTessControlImageUniforms = 0;
    Resources.maxTessEvaluationImageUniforms = 0;
    Resources.maxGeometryImageUniforms = 0;
    Resources.maxFragmentImageUniforms = 8;
    Resources.maxCombinedImageUniforms = 8;
    Resources.maxGeometryTextureImageUnits = 16;
    Resources.maxGeometryOutputVertices = 256;
    Resources.maxGeometryTotalOutputComponents = 1024;
    Resources.maxGeometryUniformComponents = 1024;
    Resources.maxGeometryVaryingComponents = 64;
    Resources.maxTessControlInputComponents = 128;
    Resources.maxTessControlOutputComponents = 128;
    Resources.maxTessControlTextureImageUnits = 16;
    Resources.maxTessControlUniformComponents = 1024;
    Resources.maxTessControlTotalOutputComponents = 4096;
    Resources.maxTessEvaluationInputComponents = 128;
    Resources.maxTessEvaluationOutputComponents = 128;
    Resources.maxTessEvaluationTextureImageUnits = 16;
    Resources.maxTessEvaluationUniformComponents = 1024;
    Resources.maxTessPatchComponents = 120;
    Resources.maxPatchVertices = 32;
    Resources.maxTessGenLevel = 64;
    Resources.maxViewports = 16;
    Resources.maxVertexAtomicCounters = 0;
    Resources.maxTessControlAtomicCounters = 0;
    Resources.maxTessEvaluationAtomicCounters = 0;
    Resources.maxGeometryAtomicCounters = 0;
    Resources.maxFragmentAtomicCounters = 8;
    Resources.maxCombinedAtomicCounters = 8;
    Resources.maxAtomicCounterBindings = 1;
    Resources.maxVertexAtomicCounterBuffers = 0;
    Resources.maxTessControlAtomicCounterBuffers = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers = 0;
    Resources.maxGeometryAtomicCounterBuffers = 0;
    Resources.maxFragmentAtomicCounterBuffers = 1;
    Resources.maxCombinedAtomicCounterBuffers = 1;
    Resources.maxAtomicCounterBufferSize = 16384;
    Resources.maxTransformFeedbackBuffers = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances = 8;
    Resources.maxCombinedClipAndCullDistances = 8;
    Resources.maxSamples = 4;
    Resources.limits.nonInductiveForLoops = 1;
    Resources.limits.whileLoops = 1;
    Resources.limits.doWhileLoops = 1;
    Resources.limits.generalUniformIndexing = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing = 1;
    Resources.limits.generalSamplerIndexing = 1;
    Resources.limits.generalVariableIndexing = 1;
    Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

// 查找着色器语言
EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type)
{
    switch (shader_type)
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        return EShLangVertex;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return EShLangTessControl;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return EShLangTessEvaluation;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
        return EShLangGeometry;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        return EShLangFragment;
    case VK_SHADER_STAGE_COMPUTE_BIT:
        return EShLangCompute;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
    case VK_SHADER_STAGE_MISS_BIT_KHR:
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
        // 光线追踪着色器，可能需要特定处理
        return EShLangRayGen;
    default:
        return EShLangVertex;
    }
}

// 主函数
int main()
{
    return sample_main();
}