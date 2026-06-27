/*- C --------------------------------------------------------------------------------------*\
    Version     Author          Desciption
    1.00        LiuHongWei      客户端日志工具, 对log4cplus的简单封闭, 具体参考cpp文件中test函数.
    ...
\*------------------------------------------------------------------------------------------*/

#ifndef LHW_LOG_H
#define LHW_LOG_H

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

extern log4cplus::Logger daily_rolling_file_logger;

void log4cplus_init();                              //log4cplus模块初始化

#define LHW_FATAL(LOG_MSG) LOG4CPLUS_FATAL(daily_rolling_file_logger, LOG_MSG)
#define LHW_ERROR(LOG_MSG) LOG4CPLUS_ERROR(daily_rolling_file_logger, LOG_MSG)
#define LHW_WARN(LOG_MSG) LOG4CPLUS_WARN(daily_rolling_file_logger, LOG_MSG)
#define LHW_INFO(LOG_MSG) LOG4CPLUS_INFO(daily_rolling_file_logger, LOG_MSG)
#define LHW_DEBUG(LOG_MSG) LOG4CPLUS_DEBUG(daily_rolling_file_logger, LOG_MSG)
#define LHW_TRACE(LOG_MSG) LOG4CPLUS_TRACE(daily_rolling_file_logger, LOG_MSG)

#endif // LHW_LOG_H
