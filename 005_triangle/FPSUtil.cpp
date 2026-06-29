#include "FPSUtil.h"
#include <chrono>
#include <thread>
#include "log4cplus_log.h"
int FPSUtil::FPSCount;
long long FPSUtil::startTime;
float FPSUtil::currFPS;
long long FPSUtil::beforeTime;

long long currentTimeMillis() {
    auto now = std::chrono::system_clock::now();
    auto epoch_diff = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch_diff);
    return static_cast<long long>(ms.count());
}

void FPSUtil::init() {
    FPSCount = 0;
    startTime = currentTimeMillis();
}

void FPSUtil::calFPS() {
    FPSCount++;
    if (FPSCount == 100) {
        FPSCount = 0;
        long long endTime = currentTimeMillis();
        currFPS = 1000.0 / ((endTime - startTime) / 100.0);
        startTime = endTime;
        LHW_INFO("FPS: " << FPSUtil::currFPS);
    }
}

void FPSUtil::before() {
    beforeTime = currentTimeMillis();
}

void FPSUtil::after(int dstFPS) {
    //计算指定FPS对应的每帧毫秒数
    int dstSpan = (int) (1000 / dstFPS) + 1;
    //计算此帧耗时
    long long span = currentTimeMillis() - beforeTime;
    //如果此帧耗时小于指定FPS对应的每帧毫秒数则加入动态时间休眠
    if (span < dstSpan) {
        std::this_thread::sleep_for(std::chrono::milliseconds(dstSpan - span));
    }
}