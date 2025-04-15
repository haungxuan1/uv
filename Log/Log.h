#pragma once
#include "log4cplus\log4cplus.h"

extern  log4cplus::Logger g_logger;
#define LOG_TRACE(log)			LOG4CPLUS_TRACE(g_logger, log)
#define LOG_DEBUG(log)			LOG4CPLUS_DEBUG(g_logger, log)
#define LOG_INFO(log)			LOG4CPLUS_INFO(g_logger, log)
#define LOG_WARN(log)			LOG4CPLUS_WARN(g_logger, log)
#define LOG_ERROR(log)			LOG4CPLUS_ERROR(g_logger, log)
#define LOG_FATAL(log)			LOG4CPLUS_FATAL(g_logger, log)

namespace Log
{
	void Reg(int nLogLevel);

	void Unreg();
}