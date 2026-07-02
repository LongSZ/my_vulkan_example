#include "log4cplus_log.h"

#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/configurator.h>
using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::thread;

#include <iostream>
#include <memory>
#include <iomanip>
using namespace std;


#ifdef Q_OS_WIN32
#pragma comment(lib, "advapi32.lib")            //log4cplus使用
#endif

Logger daily_rolling_file_logger;

static void test() {
    LHW_TRACE("This i s" << "just a t" << "est." <<  std::endl);
    LHW_DEBUG("This is a bool:" << true);
    LHW_INFO ("This is a char:" << 'x');
    LHW_WARN ("This is a int:" << 1000);
    LHW_ERROR("This is a long(hex):" << std::hex  << 100000000);
    LHW_FATAL("This is a double:" <<  std::setprecision(5) << 1.2345234234);
    
    LHW_TRACE("测试中文 TRACE");
    LHW_DEBUG("测试中文 DEBUG");
    LHW_INFO ("测试中文 INFO");
    LHW_WARN ("测试中文 WARN");
    LHW_ERROR("测试中文 ERROR %d");
    LHW_FATAL("测试中文 FATAL %s");
}

void log4cplus_init() {
    log4cplus::initialize();
    daily_rolling_file_logger =  Logger::getInstance(LOG4CPLUS_TEXT("daily_rolling_file_logger"));
    LogLog::getLogLog()->setInternalDebugging(true);
    Logger root = Logger::getRoot();
    try {
        ConfigureAndWatchThread configureThread(LOG4CPLUS_TEXT("log4cplus.properties"), 1 * 1000);

        //测试 log4cplus 库的 demo 接口函数
        //test();
    }
    catch(...) {
        LOG4CPLUS_FATAL(root, "Exception  occured...");
    }
}
