#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "log4cplus_log.h"

void error_callback(int error, const char* description) {
    LHW_ERROR("error_callback: " << description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}

void cursor_position_callback(GLFWwindow* window, double x, double y) {

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

    //3> glfw window creation
    GLFWwindow* s_pWindow = glfwCreateWindow(1024, 1024, "Titile", NULL, NULL);
    if (!s_pWindow) {
        LHW_ERROR("glfwCreateWindow failed !");
        glfwTerminate();
        return ;
    }

    glfwSetKeyCallback(s_pWindow, key_callback);
    glfwSetFramebufferSizeCallback(s_pWindow, framebuffer_size_callback);
    glfwSetMouseButtonCallback(s_pWindow, mouse_button_callback);
    glfwSetCursorPosCallback(s_pWindow, cursor_position_callback);
    glfwSetScrollCallback(s_pWindow, scroll_callback);

    LHW_ERROR("初始化成功");
}



//#include <log4cplus/layout.h>
//#include <log4cplus/fileappender.h>
//#define LOG_TRACE(log) LOG4CPLUS_TRACE(log, log)
//#define LOG_DEBUG(log) LOG4CPLUS_DEBUG(log, log)
//#define LOG_INFO(log)  LOG4CPLUS_INFO(log, log)
//#define LOG_WARN(log)  LOG4CPLUS_WARN(log, log)
//#define LOG_ERROR(log) LOG4CPLUS_ERROR(log, log)
//#define LOG_FATAL(log) LOG4CPLUS_FATAL(log, log)

int main() {
    log4cplus_init();

    init_glfw_window();

    //log4cplus::Initializer initializer;

    //// 加载配置文件
    //try {
    //    log4cplus::PropertyConfigurator::doConfigure(
    //        LOG4CPLUS_TEXT("log4cplus.properties"));
    //}
    //catch (...) {
    //    std::cerr << "Failed to load log4cplus.properties" << std::endl;
    //    return 1;
    //}

    //// 获取不同 Logger
    //log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
    //log4cplus::Logger mainLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
    ////log4cplus::Logger dbLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Database"));

    //LOG4CPLUS_INFO(rootLogger, "Root logger info");
    //LOG4CPLUS_DEBUG(mainLogger, "Main debug: value=" << 3.14);
    ////LOG4CPLUS_WARN(dbLogger, "Database warning");

    return 0;
}


