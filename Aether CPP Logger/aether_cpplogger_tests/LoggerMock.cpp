#include "pch.h"
#include "LoggerMock.h"

namespace aether_cpplogger_tests
{
	std::string LoggerMock::createAppDataPathTest(const std::string& application, const std::string& domain)
	{
		return aether_cpplogger::Logger::createAppDataPath(application, domain);
	}

	std::string LoggerMock::createMessageSeverityPrefixTest(const aether_cpplogger::LogSeverity severity)
	{
		return aether_cpplogger::Logger::createMessageSeverityPrefix(severity);
	}

	std::string LoggerMock::createMessageTimePrefixTest(const aether_cpplogger::Logger::DateTime& dateTime)
	{
		return aether_cpplogger::Logger::createMessageTimePrefix(dateTime);
	}

	void LoggerMock::writeLogToConsoleTest(std::string_view message)
	{
		return aether_cpplogger::Logger::writeLogToConsole(message);
	}

	void LoggerMock::writeLogToFileTest(std::string_view message, const aether_cpplogger::Logger::DateTime& dateTime)
	{
		return aether_cpplogger::Logger::writeLogToFile(message, dateTime);
	}

	void LoggerMock::notifyReceiversTest(std::string_view message)
	{
		return aether_cpplogger::Logger::notifyReceivers(message);
	}

	aether_cpplogger::Logger::DateTime LoggerMock::currentDateTimeTest()
	{
		return aether_cpplogger::Logger::currentDateTime();
	}

	void LoggerMock::checkLogPathTest()
	{
		aether_cpplogger::Logger::checkLogPath();
	}

	std::string LoggerMock::checkLogFileTest(const aether_cpplogger::Logger::DateTime& dateTime)
	{
		return aether_cpplogger::Logger::checkLogFile(dateTime);
	}

	void LoggerMock::uninitializeLogger()
	{
		aether_cpplogger::Logger::uninitializeLogger();
	}
}