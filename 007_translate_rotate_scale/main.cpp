#include "MyVulkanManager.h"
#include "FPSUtil.h"
#include "log4cplus_log.h"
#include <iostream>

int xPre;
int yPre;
double xDis;
double yDis;
bool isLeftMousePressed = false;
bool firstMouse = true; // 用于防止初始跳转

void error_callback(int error, const char* description) {
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isLeftMousePressed = true;
            std::cout << "[Event] Left Mouse Button Pressed" << std::endl;

            // 获取当前光标位置作为起始点
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xPre = xpos;
            yPre = ypos;
        }
        else if (action == GLFW_RELEASE) {
            isLeftMousePressed = false;
            std::cout << "[Event] Left Mouse Button Released" << std::endl;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    if (!isLeftMousePressed) {
        return;
    }

    xDis = x - xPre;                                                //计算触控点x位移
    yDis = y - yPre;                                                //计算触控点y位移
    MyVulkanManager::xAngle += yDis / 10;                            //计算x轴旋转角
    MyVulkanManager::yAngle += xDis / 10;                            //计算y轴旋转角
    xPre = x;                                                    //记录触控点x坐标
    yPre = y;                                                    //记录触控点y坐标

    std::cout << "[Drag] Moving: xDis=" << xDis << ", yDis=" << yDis << std::endl;
}

void scroll_callback(GLFWwindow* window, double x, double y) {
    
}

void init_glfw_window() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        LHW_ERROR("glfwInit failed !");
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    //3> glfw window creation
    GLFWwindow* s_pWindow = glfwCreateWindow(MyVulkanManager::screenWidth, MyVulkanManager::screenHeight, "Titile", NULL, NULL);
    if (!s_pWindow) {
        LHW_ERROR("glfwCreateWindow failed !");
        glfwTerminate();
        return;
    }

    glfwSetKeyCallback(s_pWindow, key_callback);
    glfwSetFramebufferSizeCallback(s_pWindow, framebuffer_size_callback);
    glfwSetMouseButtonCallback(s_pWindow, mouse_button_callback);
    glfwSetCursorPosCallback(s_pWindow, cursor_position_callback);
    glfwSetScrollCallback(s_pWindow, scroll_callback);

    MyVulkanManager::s_pWindow = s_pWindow;
}

int main() {
    log4cplus_init();

    init_glfw_window();

    MyVulkanManager::init_vulkan_instance();//创建Vulkan实例
    MyVulkanManager::enumerate_vulkan_phy_devices();//获取物理设备列表
    MyVulkanManager::create_vulkan_devices();//创建逻辑设备
    MyVulkanManager::create_vulkan_CommandBuffer();//创建命令缓冲
    MyVulkanManager::init_queue();//获取设备中支持图形工作的队列
    MyVulkanManager::create_vulkan_swapChain();//初始化交换链
    MyVulkanManager::create_vulkan_DepthBuffer();//创建深度缓冲
    MyVulkanManager::create_render_pass();//创建渲染通道
    MyVulkanManager::create_frame_buffer();//创建帧缓冲
    MyVulkanManager::createDrawableObject();//创建绘制用的物体
    MyVulkanManager::initPipeline();//初始化渲染管线
    MyVulkanManager::createFence();//创建栅栏
    MyVulkanManager::initPresentInfo();//初始化呈现信息
    MyVulkanManager::initMatrix();//初始化基本变换矩阵、摄像机矩阵、投影矩阵

    FPSUtil::init();
    while (!glfwWindowShouldClose(MyVulkanManager::s_pWindow)) {
        glfwSwapBuffers(MyVulkanManager::s_pWindow);
        glfwPollEvents();

        MyVulkanManager::drawObject();//执行绘制
    }

    MyVulkanManager::destroyFence();//销毁栅栏
    MyVulkanManager::destroyPipeline();//销毁管线
    MyVulkanManager::destroyDrawableObject();//销毁绘制用物体
    MyVulkanManager::destroy_frame_buffer();//销毁帧缓冲
    MyVulkanManager::destroy_render_pass();//销毁渲染通道相关
    MyVulkanManager::destroy_vulkan_DepthBuffer();//销毁深度缓冲相关
    MyVulkanManager::destroy_vulkan_swapChain();//销毁交换链相关
    MyVulkanManager::destroy_vulkan_CommandBuffer();//销毁命令缓冲
    MyVulkanManager::destroy_vulkan_devices();//销毁逻辑设备
    MyVulkanManager::destroy_vulkan_instance();//销毁Vulkan 实例
}


