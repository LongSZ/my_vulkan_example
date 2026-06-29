#include "MyVulkanManager.h"
#include "log4cplus_log.h"

int main() {
    log4cplus_init();

    MyVulkanManager::doVulkan();

    return 0;
}


