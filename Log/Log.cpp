#include "pch.h"
#include "Log.h"
#include <string>

log4cplus::Logger g_logger = log4cplus::Logger::getInstance(L"logmain");

namespace Log
{
	void Reg(int nLogLevel)
	{
		static log4cplus::Initializer initializer;

		std::locale::global(std::locale("chs"));

		log4cplus::helpers::SharedObjectPtr<log4cplus::Appender> lpAppend(new log4cplus::RollingFileAppender(L"log.log", 1024 * 1024 * 10, 10, true, true));

//		std::wstring pattern = L"%D{%c}[%t | %p] - %M[%L]: %m%n";

		std::wstring pattern = L"%D{%Y/%m/%d %H:%M:%S:%Q}[%t | %p] - %M[%L]: %m%n";

		log4cplus::PatternLayout* lpLayout = new log4cplus::PatternLayout(pattern);

		lpAppend->setLayout(std::unique_ptr<log4cplus::Layout>(lpLayout));

		g_logger.addAppender(lpAppend);

		g_logger.setLogLevel(nLogLevel);
	}

	void Unreg()
	{
		log4cplus::Logger::shutdown();

		log4cplus::deinitialize();
	}
}