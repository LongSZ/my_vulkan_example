#pragma once
#include <vector>
#include <cstdint>
#include <vulkan/vulkan.h>

/// @brief 编译GLSL源码为SPIR-V二进制
/// @param shader_type 着色器阶段（VK_SHADER_STAGE_VERTEX_BIT等）
/// @param pshader 以\0结尾的GLSL源码字符串
/// @param spirv 输出SPIR-V uint32数组
/// @return 编译成功返回true，失败false
bool GLSLtoSPV(VkShaderStageFlagBits shader_type, const char* pshader,
    std::vector<uint32_t>& spirv);

/// 全局一次性初始化glslang（程序启动调用一次即可）
void glslangInit();
/// 全局释放glslang资源（程序退出调用一次）
void glslangDeinit();