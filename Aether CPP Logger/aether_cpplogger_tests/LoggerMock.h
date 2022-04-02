#pragma once
#include "..\aether_cpplogger\Logger.h"

namespace aether_cpplogger_tests
{
	class LoggerMock : public aether_cpplogger::Logger
	{
	public:
		static std::string createAppDataPathTest(const std::string& application, const std::string& domain);

		static std::string createMessageSeverityPrefixTest(const aether_cpplogger::LogSeverity severity);
		static std::string createMessageTimePrefixTest(const aether_cpplogger::Logger::DateTime& dateTime);

		static void writeLogToConsoleTest(std::string_view message);
		static void writeLogToFileTest(std::string_view message, const aether_cpplogger::Logger::DateTime& dateTime);
		static void notifyReceiversTest(std::string_view message);

		static aether_cpplogger::Logger::DateTime currentDateTimeTest();
		static void checkLogPathTest();
		static std::string checkLogFileTest(const aether_cpplogger::Logger::DateTime& dateTime);

		static void uninitializeLogger();
	};
}