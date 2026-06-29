#include "ShaderCompileUtil.h"
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Include/ResourceLimits.h>

// 将 VkShaderStageFlagBits 映射到 glslang 的 EShLanguage
static EShLanguage VkStageToGlslang(VkShaderStageFlagBits stage) {
    switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:                  return EShLangVertex;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:    return EShLangTessControl;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return EShLangTessEvaluation;
    case VK_SHADER_STAGE_GEOMETRY_BIT:                return EShLangGeometry;
    case VK_SHADER_STAGE_FRAGMENT_BIT:                return EShLangFragment;
    case VK_SHADER_STAGE_COMPUTE_BIT:                 return EShLangCompute;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:              return EShLangRayGen;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:             return EShLangAnyHit;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:         return EShLangClosestHit;
    case VK_SHADER_STAGE_MISS_BIT_KHR:                return EShLangMiss;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:        return EShLangIntersect;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:            return EShLangCallable;
    case VK_SHADER_STAGE_TASK_BIT_EXT:                return EShLangTask;
    case VK_SHADER_STAGE_MESH_BIT_EXT:                return EShLangMesh;
    default:                                          return EShLangVertex;
    }
}

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char* pshader,
    std::vector<unsigned int>& spirv) 
{
    if (!pshader) return false;

    if (!glslang::InitializeProcess()) {
        return false;
    }

    const EShLanguage stage = VkStageToGlslang(shader_type);

    glslang::TShader shader(stage);
    const char* shaderStrings[1] = { pshader };
    shader.setStrings(shaderStrings, 1);

    // 设置编译环境：Vulkan 1.3 + SPIR-V 1.6
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_6);
    shader.setEntryPoint("main");

    // 修正：使用 GetDefaultResource() 获取默认资源限制
    const TBuiltInResource* resources = GetDefaultResources();

    constexpr int defaultVersion = 450;
    constexpr EProfile defaultProfile = ENoProfile;
    constexpr bool forceDefaultVersionAndProfile = false;
    constexpr bool forwardCompatible = false;
    const EShMessages messages = static_cast<EShMessages>(
        EShMsgSpvRules | EShMsgVulkanRules | EShMsgDebugInfo
        );

    // 修正：preprocess 需要 8 个参数，最后一个必须是 Includer&
    glslang::TShader::ForbidIncluder includer;
    std::string preprocessedGLSL;
    if (!shader.preprocess(resources, defaultVersion, defaultProfile,
        forceDefaultVersionAndProfile, forwardCompatible,
        messages, &preprocessedGLSL, includer)) {
        glslang::FinalizeProcess();
        return false;
    }

    const char* preprocessedStrings[1] = { preprocessedGLSL.c_str() };
    shader.setStrings(preprocessedStrings, 1);

    // parse 也需要 Includer&，但可以用带默认参数的重载
    if (!shader.parse(resources, defaultVersion, defaultProfile,
        forceDefaultVersionAndProfile, forwardCompatible, messages)) {
        glslang::FinalizeProcess();
        return false;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messages)) {
        glslang::FinalizeProcess();
        return false;
    }

    glslang::SpvOptions spvOptions;
    spvOptions.generateDebugInfo = true;
    spvOptions.stripDebugInfo = false;
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = false;
    spvOptions.disassemble = false;
    spvOptions.validate = true;

    glslang::TIntermediate* intermediate = program.getIntermediate(stage);
    if (!intermediate) {
        glslang::FinalizeProcess();
        return false;
    }

    glslang::GlslangToSpv(*intermediate, spirv, &spvOptions);

    glslang::FinalizeProcess();
    return !spirv.empty();
}