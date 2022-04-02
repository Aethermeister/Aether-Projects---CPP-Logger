#include "Logger.h"
#include "LoggerException.h"

#include <iostream>
#include <algorithm>

#include <filesystem>
#include <fstream>

/**
 * @brief 1MB default log file size limit
*/
constexpr int DEFAULT_SIZE_LIMIT = 1048576;

namespace aether_cpplogger
{
	bool Logger::s_isInitialized = false;
	std::string Logger::s_logPath = std::string();
	bool Logger::s_printLog = false;
	LogSeverity Logger::s_severityLimit = LogSeverity::ERROR;
	int Logger::s_sizeLimit = DEFAULT_SIZE_LIMIT;
	std::vector<Receiver*> Logger::s_receivers = std::vector<Receiver*>();

	void Logger::log(const std::string& message, const LogSeverity severity)
	{
		//Check the Logger initialization state
		if (!s_isInitialized)
		{
			throw LoggerException("Logger is not initialized");
		}

		//Check whether the severity of this log exceeds the severity limit
		if (severity > s_severityLimit)
		{
			return;
		}

		//Get the current DateTime and format the log message
		const auto& dateTime = currentDateTime();
		std::string fullMessage = createMessageSeverityPrefix(severity) + createMessageTimePrefix(dateTime) + message;

		writeLogToConsole(fullMessage);
		writeLogToFile(fullMessage, dateTime);

		notifyReceivers(message);
	}

	std::string Logger::createAppDataPath(std::string_view application, std::string_view domain)
	{
		//Get the Roaming AppData folder
		char* appdataFolder = nullptr;
		_dupenv_s(&appdataFolder, nullptr, "APPDATA");

		if (appdataFolder)
		{
			//If domain was given add it to the log path
			std::string specifiedApplicationFolder;
			if (!domain.empty())
			{
				specifiedApplicationFolder += "\\";
				specifiedApplicationFolder += domain;
			}

			//Add the application name to the log path
			specifiedApplicationFolder += "\\";
			specifiedApplicationFolder += application;

			const std::string& appdataFolderPath = std::string(appdataFolder) + specifiedApplicationFolder + "\\logs";
			delete appdataFolder;

			return appdataFolderPath;
		}
		else
		{
			//Throw exception if the AppData Roaming folder could not be retrieved
			throw LoggerException("Logger could not be initialized");
		}
	}

	std::string Logger::createMessageSeverityPrefix(const LogSeverity severity)
	{
		std::string severityPrefix;
		if (severity == LogSeverity::INFO)
		{
			severityPrefix = "[INFO]\t\t";
		}
		else if (severity == LogSeverity::WARNING)
		{
			severityPrefix = "[WARNING]\t";
		}
		else if (severity == LogSeverity::ERROR)
		{
			severityPrefix = "[ERROR]\t\t";
		}
		else if (severity == LogSeverity::DEBUG)
		{
			severityPrefix = "[DEBUG]\t\t";
		}
		else if (severity == LogSeverity::TRACE)
		{
			severityPrefix = "[TRACE]\t\t";
		}

		return severityPrefix;
	}

	std::string aether_cpplogger::Logger::createMessageTimePrefix(const DateTime& dateTime)
	{
		return dateTime.currentTimeString() + "\t\t";
	}

	std::string Logger::createDetailedMessage(const std::string& message, std::string_view source, const int line)
	{
		std::string detailedMessage = message;

		//Add the source file to the message
		detailedMessage += "\t\tSOURCE: ";
		detailedMessage += source;

		//Add the source line to the message
		detailedMessage += "\t\tLINE: ";
		detailedMessage += std::to_string(line);

		return detailedMessage;
	}

	void Logger::writeLogToConsole(std::string_view message)
	{
		if (s_printLog)
		{
			std::cout << message << std::endl;
		}
	}

	void Logger::writeLogToFile(std::string_view message, const DateTime& dateTime)
	{
		try
		{
			checkLogPath();
			const auto& currentLogFileName = checkLogFile(dateTime);

			std::ofstream outLogFile;
			outLogFile.open(s_logPath + "\\" + currentLogFileName, std::ios::app | std::ios::out);
			if (outLogFile.is_open())
			{
				outLogFile << message << std::endl;
				outLogFile.close();
			}
			else
			{
				std::cerr << "Log file could not be opened" << std::endl;
			}
		}
		catch (const std::filesystem::filesystem_error& ex)
		{
			const std::string exceptionMessage = "!!!Filesystem error!!!" + std::string(ex.what());
			throw LoggerException(exceptionMessage);
		}
		catch (const std::ofstream::failure& ex) {
			const std::string exceptionMessage = "!!!Log file writing error!!!" + std::string(ex.what());
			throw LoggerException(exceptionMessage);
		}
	}

	void Logger::notifyReceivers(std::string_view message)
	{
		for (const auto reciever : s_receivers)
		{
			reciever->onReceive(message);
		}
	}

	Logger::DateTime Logger::currentDateTime()
	{
		const time_t now = time(nullptr);
		tm ltm;
		localtime_s(&ltm, &now);

		DateTime dt;
		dt.Year = 1900 + ltm.tm_year;
		dt.Month = 1 + ltm.tm_mon;
		dt.Day = ltm.tm_mday;
		dt.Hours = ltm.tm_hour;
		dt.Minutes = ltm.tm_min;
		dt.Seconds = ltm.tm_sec;

		return dt;
	}

	void Logger::checkLogPath()
	{
		//Check the existence of the log path
		//and create it if it doe not exist
		if (!std::filesystem::exists(s_logPath))
		{
			std::filesystem::create_directories(s_logPath);
		}
	}

	std::string Logger::checkLogFile(const DateTime& dateTime)
	{
		std::string filename;
		const auto& nameBase = dateTime.currentDateString();
		int logFileIndex = 1;

		//Check and retrieve the exact name of the log file
		while (checkLogFileIndexing(nameBase, logFileIndex, filename));

		return filename;
	}

	bool Logger::checkLogFileIndexing(std::string_view nameBase, int& index, std::string& filename)
	{
		//Index check attempt counter is used to avoid infinite loop
		//Throw LoggerException if the counter reaches the limit
		if (const int maxAttempt = 99999; index > maxAttempt)
		{
			throw LoggerException("Log file index checking exceeded limit");
		}

		//Use the given name base as base of the log file name and modify it according to the current index
		filename = nameBase;
		if (index > 1)
		{
			filename += "_" + std::to_string(index);
		}
		filename += ".log";

		//Check the existence of the currently checked log file
		//If it does not exist than no further size check is needed and return
		if (!std::filesystem::exists(s_logPath + "\\" + filename))
		{
			return false;
		}

		//Check the size of the log file
		if (const auto fileSize = std::filesystem::file_size(s_logPath + "\\" + filename);
			fileSize < s_sizeLimit)
		{
			return false;
		}

		index += 1;

		//Return true to countinue the loop
		return true;
	}

	void Logger::uninitializeLogger()
	{
		s_isInitialized = false;
	}

	void Logger::init(std::string_view logPath)
	{
		s_isInitialized = true;
		s_logPath = logPath;
	}

	void Logger::init(std::string_view logPath, const bool printLog, const LogSeverity severityLimit, const int sizeLimit)
	{
		s_isInitialized = true;
		s_logPath = logPath;
		s_printLog = printLog;
		s_severityLimit = severityLimit;
		s_sizeLimit = sizeLimit;
	}

	void Logger::init(const std::string& application, const std::string& domain)
	{
		s_logPath = createAppDataPath(application, domain);

		s_isInitialized = true;
	}

	void Logger::init(const std::string& application, const std::string& domain, const bool printLog, const LogSeverity severityLimit, const int sizeLimit)
	{
		s_logPath = createAppDataPath(application, domain);

		s_printLog = printLog;
		s_severityLimit = severityLimit;
		s_sizeLimit = sizeLimit;

		s_isInitialized = true;
	}

	void Logger::addReceiver(Receiver* receiver)
	{
		s_receivers.push_back(receiver);
	}

	void Logger::removeReceiver(Receiver* receiver)
	{
		s_receivers.erase(std::find(s_receivers.begin(), s_receivers.end(), receiver));
	}

	void Logger::clearReceivers()
	{
		s_receivers.clear();
	}

	void Logger::logInfo(const std::string& message)
	{
		log(message, LogSeverity::INFO);
	}

	void Logger::logWarning(const std::string& message)
	{
		log(message, LogSeverity::WARNING);
	}

	void Logger::logError(const std::string& message)
	{
		log(message, LogSeverity::ERROR);
	}

	void Logger::logDebug(const std::string& message, std::string_view source, const int line)
	{
		const auto& detailedMessage = createDetailedMessage(message, source, line);

		log(detailedMessage, LogSeverity::DEBUG);
	}

	void Logger::logTrace(const std::string& message, std::string_view source, const int line)
	{
		const auto& detailedMessage = createDetailedMessage(message, source, line);

		log(detailedMessage, LogSeverity::TRACE);
	}
}