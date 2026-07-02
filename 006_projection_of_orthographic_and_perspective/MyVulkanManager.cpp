#include <vulkan/vulkan.h>
#include "MatrixState3D.h"
#include "MyVulkanManager.h"
#include "FileUtil.h"
#include "HelpFunction.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include "FPSUtil.h"
#include "SixPointedStar.h"
#include "log4cplus_log.h"

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

uint32_t MyVulkanManager::screenWidth = 1920;
uint32_t MyVulkanManager::screenHeight = 1080;
GLFWwindow* MyVulkanManager::s_pWindow = nullptr;

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
DrawableObjectCommonLight* MyVulkanManager::objForDraw;
float MyVulkanManager::xAngle = 0;
float MyVulkanManager::yAngle = 0;

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

void MyVulkanManager::init_vulkan_instance() {
    instanceExtensionNames = getRequiredExtensions();
    instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);//初始化所需实例扩展名称列表

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "HelloVulkan";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "HelloVulkan";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = instanceExtensionNames.size();
    inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;
    VkResult result;

    result = vkCreateInstance(&inst_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        LHW_ERROR("Vulkan实例创建失败!");
    }
}


void MyVulkanManager::destroy_vulkan_instance() {
    vkDestroyInstance(instance, NULL);
    LHW_INFO("Vulkan实例销毁完毕!");
}


void MyVulkanManager::enumerate_vulkan_phy_devices() {
    gpuCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    assert(result == VK_SUCCESS);
    LHW_INFO("[Vulkan硬件设备数量为%d个]", gpuCount);
    gpus.resize(gpuCount);
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    assert(result == VK_SUCCESS);
    vkGetPhysicalDeviceMemoryProperties(gpus[0], &memoryroperties);
}


void MyVulkanManager::create_vulkan_devices() {
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, NULL);
    LHW_INFO("Vulkan硬件设备0支持的队列家族数量为 " << queueFamilyCount);
    queueFamilyprops.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount,
        queueFamilyprops.data());
    LHW_INFO("[成功获取Vulkan硬件设备0支持的队列家族属性列表]");

    VkDeviceQueueCreateInfo queueInfo = {};
    bool found = false;
    for (unsigned int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueInfo.queueFamilyIndex = i;
            queueGraphicsFamilyIndex = i;
            LHW_INFO("支持GRAPHICS工作的一个队列家族的索引为 " << i);
            LHW_INFO("此家族中的实际队列数量是 " << queueFamilyprops[i].queueCount);
            found = true;
            break;
        }
    }

    float queue_priorities[1] = { 0.0 };
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    queueInfo.queueFamilyIndex = queueGraphicsFamilyIndex;
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = deviceExtensionNames.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    deviceInfo.pEnabledFeatures = NULL;
    VkResult result = vkCreateDevice(gpus[0], &deviceInfo, NULL, &device);
    assert(result == VK_SUCCESS);
}


void MyVulkanManager::destroy_vulkan_devices() {
    vkDestroyDevice(device, NULL);
    LHW_INFO("逻辑设备销毁完毕！");
}

void MyVulkanManager::create_vulkan_CommandBuffer() {

    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkResult result = vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmdPool);
    assert(result == VK_SUCCESS);

    VkCommandBufferAllocateInfo cmdBAI = {};
    cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBAI.pNext = NULL;
    cmdBAI.commandPool = cmdPool;
    cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBAI.commandBufferCount = 1;
    result = vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);

    assert(result == VK_SUCCESS);
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = NULL;
    cmd_bufs[0] = cmdBuffer;

    VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();
    *pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].pWaitDstStageMask = pipe_stage_flags;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;
}


void MyVulkanManager::destroy_vulkan_CommandBuffer() {
    VkCommandBuffer cmdBufferArray[1] = { cmdBuffer };
    vkFreeCommandBuffers(
        device,
        cmdPool,
        1,
        cmdBufferArray
    );
    vkDestroyCommandPool(device, cmdPool, NULL);
}


void MyVulkanManager::create_vulkan_swapChain() {
    if (glfwCreateWindowSurface(instance, s_pWindow, nullptr, &surface) != VK_SUCCESS) {
        LHW_ERROR("failed to create window surface!");
        throw std::runtime_error("failed to create window surface!");
    }

    VkBool32* pSupportsPresent = (VkBool32*)malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], i, surface, &pSupportsPresent[i]);
        LHW_INFO("queue Family Index = " << i << ", " << (pSupportsPresent[i] == 1 ? "support" : "not support") << "display");
    }
    queueGraphicsFamilyIndex = UINT32_MAX;
    queuePresentFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if ((queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {

            if (queueGraphicsFamilyIndex == UINT32_MAX) queueGraphicsFamilyIndex = i;
            if (pSupportsPresent[i] == VK_TRUE) {
                queueGraphicsFamilyIndex = i;
                queuePresentFamilyIndex = i;
                LHW_INFO("队列家族索引=" << i << ", 同时支持Graphis（图形）和Present（呈现）工作");
                break;
            }
        }
    }
    if (queuePresentFamilyIndex == UINT32_MAX) {
        for (size_t i = 0; i < queueFamilyCount; ++i) {
            if (pSupportsPresent[i] == VK_TRUE) {
                queuePresentFamilyIndex = i;
                break;
            }
        }
    }
    free(pSupportsPresent);


    if (queueGraphicsFamilyIndex == UINT32_MAX || queuePresentFamilyIndex == UINT32_MAX) {
        LHW_ERROR("there is no Graphis or Present queue family index");
        assert(false);
    }

    uint32_t formatCount;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, NULL);
    LHW_INFO("支持的格式数量为 " << formatCount);
    VkSurfaceFormatKHR* surfFormats = (VkSurfaceFormatKHR*)malloc(
        formatCount * sizeof(VkSurfaceFormatKHR));
    formats.resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfFormats);
    for (int i = 0; i < formatCount; i++) {
        formats[i] = surfFormats[i].format;
        LHW_INFO("支持的格式[" << i << "]为 " << formats[i]);
    }
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        formats[0] = VK_FORMAT_B8G8R8A8_UNORM;
    }
    free(surfFormats);

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &surfCapabilities);
    assert(result == VK_SUCCESS);


    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount,
        NULL);
    assert(result == VK_SUCCESS);
    LHW_INFO("显示模式数量为 " << presentModeCount);

    presentModes.resize(presentModeCount);

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount,
        presentModes.data());
    for (int i = 0; i < presentModeCount; i++) {
        LHW_INFO("显示模式[" << i << "]编号为 " << presentModes[i]);
    }
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {

        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {


            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }


    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
        swapchainExtent.width = screenWidth;
        swapchainExtent.height = screenHeight;

        if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        }
        else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        }
        else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
        LHW_INFO("使用自己设置的 宽度 " << swapchainExtent.width << " 高度 " << swapchainExtent.height);
    }
    else {

        swapchainExtent = surfCapabilities.currentExtent;
        LHW_INFO("使用获取的surface能力中的 宽度 " << swapchainExtent.width << " 高度 " << swapchainExtent.height);
    }

    screenWidth = swapchainExtent.width;
    screenHeight = swapchainExtent.height;

    uint32_t desiredMinNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;

    if ((surfCapabilities.maxImageCount > 0) &&
        (desiredMinNumberOfSwapChainImages > surfCapabilities.maxImageCount)) {
        desiredMinNumberOfSwapChainImages = surfCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
        preTransform = surfCapabilities.currentTransform;
    }
    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = surface;
    swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;
    swapchain_ci.imageFormat = formats[0];
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;

    if (queueGraphicsFamilyIndex != queuePresentFamilyIndex) {
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        uint32_t queueFamilyIndices[2] = { queueGraphicsFamilyIndex, queuePresentFamilyIndex };
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
    }

    result = vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapChain);
    assert(result == VK_SUCCESS);


    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, NULL);
    assert(result == VK_SUCCESS);
    LHW_INFO("SwapChain中的Image数量为 " << swapchainImageCount);
    swapchainImages.resize(swapchainImageCount);

    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount,
        swapchainImages.data());
    assert(result == VK_SUCCESS);

    swapchainImageViews.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.flags = 0;
        color_image_view.image = swapchainImages[i];
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.format = formats[0];
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        result = vkCreateImageView(device, &color_image_view, NULL, &swapchainImageViews[i]);
        assert(result == VK_SUCCESS);
    }
}

void MyVulkanManager::destroy_vulkan_swapChain() {
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        vkDestroyImageView(device, swapchainImageViews[i], NULL);
        LHW_INFO("销毁SwapChain ImageView " << i << " 成功");
    }
    vkDestroySwapchainKHR(device, swapChain, NULL);
    LHW_INFO("销毁SwapChain成功!");
}


void MyVulkanManager::create_vulkan_DepthBuffer() {
    depthFormat = VK_FORMAT_D16_UNORM;

    VkImageCreateInfo image_info = {};
    vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &depthFormatProps);

    if (depthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
        LHW_INFO("tiling is VK_IMAGE_TILING_LINEAR.");
    }
    else if (depthFormatProps.optimalTilingFeatures &
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        LHW_INFO("tiling is VK_IMAGE_TILING_OPTIMAL.");
    }
    else {
        LHW_ERROR("VK_FORMAT_D16_UNORM is not supported!");
    }
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depthFormat;
    image_info.extent.width = screenWidth;
    image_info.extent.height = screenHeight;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depthFormat;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;

    VkResult result = vkCreateImage(device, &image_info, NULL, &depthImage);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, depthImage, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = 0;
    bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits,
        requirements_mask, &mem_alloc.memoryTypeIndex);
    assert(flag);
    LHW_INFO("mem_alloc.memoryTypeIndex: " << mem_alloc.memoryTypeIndex);
    result = vkAllocateMemory(device, &mem_alloc, NULL, &memDepth);
    assert(result == VK_SUCCESS);
    result = vkBindImageMemory(device, depthImage, memDepth, 0);
    assert(result == VK_SUCCESS);
    view_info.image = depthImage;
    result = vkCreateImageView(device, &view_info, NULL, &depthImageView);
    assert(result == VK_SUCCESS);

}


void MyVulkanManager::destroy_vulkan_DepthBuffer() {
    vkDestroyImageView(device, depthImageView, NULL);
    vkDestroyImage(device, depthImage, NULL);
    vkFreeMemory(device, memDepth, NULL);
    LHW_INFO("销毁深度缓冲相关成功!");
}


void MyVulkanManager::create_render_pass() {
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = NULL;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    VkResult result = vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, NULL,
        &imageAcquiredSemaphore);
    assert(result == VK_SUCCESS);

    VkAttachmentDescription attachments[2];
    attachments[0].format = formats[0];
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = 0;
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = &depth_reference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = NULL;
    rp_info.attachmentCount = 2;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = NULL;

    result = vkCreateRenderPass(device, &rp_info, NULL, &renderPass);
    assert(result == VK_SUCCESS);

    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = renderPass;
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = screenWidth;
    rp_begin.renderArea.extent.height = screenHeight;
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clear_values;

}

void MyVulkanManager::destroy_render_pass() {
    vkDestroyRenderPass(device, renderPass, NULL);
    vkDestroySemaphore(device, imageAcquiredSemaphore, NULL);
}


void MyVulkanManager::init_queue() {

    vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0, &queueGraphics);
}


void MyVulkanManager::create_frame_buffer() {
    VkImageView attachments[2];
    attachments[1] = depthImageView;
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = NULL;
    fb_info.renderPass = renderPass;
    fb_info.attachmentCount = 2;
    fb_info.pAttachments = attachments;
    fb_info.width = screenWidth;
    fb_info.height = screenHeight;
    fb_info.layers = 1;
    uint32_t i;
    framebuffers = (VkFramebuffer*)malloc(swapchainImageCount * sizeof(VkFramebuffer));
    assert(framebuffers);
    for (i = 0; i < swapchainImageCount; i++) {
        attachments[0] = swapchainImageViews[i];
        VkResult result = vkCreateFramebuffer(device, &fb_info, NULL, &framebuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void MyVulkanManager::destroy_frame_buffer() {

    for (int i = 0; i < swapchainImageCount; i++) {
        vkDestroyFramebuffer(device, framebuffers[i], NULL);
    }
    free(framebuffers);
}


void MyVulkanManager::createDrawableObject() {
    SixPointedStar::genStarData(0.2, 0.5, 0);
    objForDraw = new DrawableObjectCommonLight(SixPointedStar::vdata, SixPointedStar::dataByteCount,
        SixPointedStar::vCount, device, memoryroperties);
}


void MyVulkanManager::destroyDrawableObject() {
    delete objForDraw;
}

void MyVulkanManager::createFence() {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(device, &fenceInfo, NULL, &taskFinishFence);
}


void MyVulkanManager::destroyFence() {
    vkDestroyFence(device, taskFinishFence, NULL);
}

void MyVulkanManager::initPresentInfo() {
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.waitSemaphoreCount = 0;
    present.pWaitSemaphores = NULL;
    present.pResults = NULL;
}

void MyVulkanManager::initMatrix() {
    MatrixState3D::setCamera(0, 0, 2, 0, 0, 0, 0, 1, 0);                        //设置摄像机
    MatrixState3D::setInitStack();                                    //初始化基本变换矩阵
    float ratio = (float)screenWidth / (float)screenHeight;                    //计算屏幕宽高比

    MatrixState3D::setProjectOrtho(-ratio, ratio, -1, 1, 0.8f, 50);                //设置正交投影参数
    //MatrixState3D::setProjectFrustum(-ratio * 0.4, ratio * 0.4, -1 * 0.4, 1 * 0.4, 1.0f, 20);                //设置透视投影参数
    //MatrixState3D::setProjectFrustum(-ratio, ratio, -1, 1 , 0.8f, 50);
}


void MyVulkanManager::flushUniformBuffer() {
    float* vertexUniformData = MatrixState3D::getFinalMatrix();
    uint8_t* pData;
    VkResult result = vkMapMemory(device, sqsCL->memUniformBuf, 0, sqsCL->bufferByteCount, 0,
        (void**)&pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, vertexUniformData, sqsCL->bufferByteCount);
    vkUnmapMemory(device, sqsCL->memUniformBuf);
}

void MyVulkanManager::flushTexToDesSet() {
    sqsCL->writes[0].dstSet = sqsCL->descSet[0];
    vkUpdateDescriptorSets(device, 1, sqsCL->writes, 0, NULL);
}

void MyVulkanManager::drawObject() {
    FPSUtil::calFPS();
    FPSUtil::before();

    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
        imageAcquiredSemaphore, VK_NULL_HANDLE,
        &currentBuffer);
    rp_begin.framebuffer = framebuffers[currentBuffer];
    vkResetCommandBuffer(cmdBuffer, 0);
    result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);

    MyVulkanManager::flushUniformBuffer();
    MyVulkanManager::flushTexToDesSet();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    MatrixState3D::pushMatrix();                                        //保护现场
    MatrixState3D::rotate(xAngle, 1, 0, 0);                                //绕X轴旋转xAngle
    MatrixState3D::rotate(yAngle, 0, 1, 0);                                //绕Y轴旋转yAngle
    for (int i = 0; i <= 5; i++) {                                                //循环绘制所有六角星
        MatrixState3D::pushMatrix();                                    //保护现场
        MatrixState3D::translate(0, 0, i * 0.3);                            //沿Z轴平移
        objForDraw->drawSelf(cmdBuffer, sqsCL->pipelineLayout,            //绘制物体
            sqsCL->pipeline, &(sqsCL->descSet[0]));
        MatrixState3D::popMatrix();
    }                                    //恢复现场
    MatrixState3D::popMatrix();

    vkCmdEndRenderPass(cmdBuffer);
    result = vkEndCommandBuffer(cmdBuffer);

    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;

    result = vkQueueSubmit(queueGraphics, 1, submit_info, taskFinishFence);
    do {
        result = vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vkResetFences(device, 1, &taskFinishFence);
    present.pImageIndices = &currentBuffer;
    result = vkQueuePresentKHR(queueGraphics, &present);
    FPSUtil::after(60);
}

void MyVulkanManager::initPipeline() {
    sqsCL = new ShaderQueueSuit_Common(&device, renderPass, memoryroperties);
}

void MyVulkanManager::destroyPipeline() {
    delete sqsCL;
}