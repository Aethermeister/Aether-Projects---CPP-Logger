#pragma once
#include "Receiver.h"

#include <string>
#include <vector>
#include <ctime>

#define AETHER_LOG_INIT_1(logPath) aether_cpplogger::Logger::init(logPath)
#define AETHER_LOG_INIT_1A(logPath, printLog, severityLimit, sizeLimit) aether_cpplogger::Logger::init(logPath, printLog, severityLimit, sizeLimit)

#define AETHER_LOG_INIT_2(application, domain) aether_cpplogger::Logger::init(application, domain)
#define AETHER_LOG_INIT_2A(application, domain, printLog, severityLimit, sizeLimit) aether_cpplogger::Logger::init(application, domain, printLog, severityLimit, sizeLimit)

#define AETHER_LOG_INFO(message) aether_cpplogger::Logger::logInfo(message)
#define AETHER_LOG_WARNING(message) aether_cpplogger::Logger::logWarning(message)
#define AETHER_LOG_ERROR(message) aether_cpplogger::Logger::logError(message)
#define AETHER_LOG_DEBUG(message) aether_cpplogger::Logger::logDebug(message, __FILE__, __LINE__)
#define AETHER_LOG_TRACE(message) aether_cpplogger::Logger::logTrace(message, __FILE__, __LINE__)

namespace aether_cpplogger
{
	/**
	 * @brief Severity enum class for the Logger.
		Each log made by the Logger has a severity value.
		The behaviour of the Logger can be changed according to the selected severity level
	*/
	enum class LogSeverity
	{
		INFO,
		WARNING,
		ERROR,
		DEBUG,
		TRACE
	};

	/**
	 * @brief A simple Logger class brought by Aether Projects.
	 * 
	 * Easy initialization and usage.
	 * Information is logged into files and (optionally) the console.
	 * Logged information is also forwarded to the attached receivers
	*/
	class __declspec(dllexport) Logger
	{
	public:
		/**
		 * @brief A structure to handle date and time data
		*/
		struct DateTime
		{
			/**
			 * @brief The year this DateTime reflects
			*/
			int Year;
			/**
			 * @brief The month this DateTime reflects
			*/
			int Month;
			/**
			 * @brief The day this DateTime reflects
			*/
			int Day;
			/**
			 * @brief The hours this DateTime reflects
			*/
			int Hours;
			/**
			 * @brief The minutes this DateTime reflects
			*/
			int Minutes;
			/**
			 * @brief The seconds this DateTime reflects
			*/
			int Seconds;

			/**
			 * @brief Formats the date related variables into a string
			 * @return The formatted date related variables as a string e.: 1900-10-13
			*/
			std::string currentDateString() const
			{
				return std::to_string(Year) + "-" +
					std::to_string(Month) + "-" +
					std::to_string(Day);
			}

			/**
			 * @brief Formats the time related variables into a string
			 * @return The formatted time related variables as a string e.: 21:32:18
			*/
			std::string currentTimeString() const
			{
				return std::to_string(Hours) + ":" +
					std::to_string(Minutes) + ":" +
					std::to_string(Seconds);
			}
		};

	private:
		/**
		 * @brief static flag indicating the initialization state of the Logger
		*/
		static bool s_isInitialized;
		/**
		 * @brief static string which holds the path to the log destination folder
		*/
		static std::string s_logPath;
		/**
		 * @brief static flag which indicates whether the logged information should be printed on the console
		*/
		static bool s_printLog;
		/**
		 * @brief static enum type variable which indicates the log severity limit. Logs with severity over the limit are discarded
		*/
		static LogSeverity s_severityLimit;
		/**
		 * @brief static integer which defines the maximum size of a log file
		*/
		static int s_sizeLimit;

		/**
		 * @brief static vector of Receiver type pointers. These stored objects are notified upon each log made
		*/
		static std::vector<Receiver*> s_receivers;

	protected:
		/**
		 * @brief Creates a log according to the given severity
		 *
		 * @param message The message to be logged
		 * @param severity The severity of this log
		*/
		static void log(const std::string& message, const LogSeverity severity = LogSeverity::INFO);
		/**
		 * @brief Creates a string which points to the AppData folder with the addition of the user given domain and application values
		 * 
		 * @param application The name of the appliation this log is made for. The name of the application is used in the folder structure.
		 * @param domain (optional)The domain of the application this log is made for. The domain of the application is used in the folder structure.
		 * 
		 * @return The path to the log folder in the AppData. e.g.: %APPDATA%\\@p domain (optional)\\@p application\\logs
		*/
		static std::string createAppDataPath(std::string_view, std::string_view domain);
		/**
		 * @brief Creates a formatted severity prefix for the log message according to the given severity
		 *
		 * @param severity The severity of the given log
		 *
		 * @return The severity prefix for the log message
		*/
		static std::string createMessageSeverityPrefix(const LogSeverity severity);
		/**
		 * @brief Creates a formatted time prefix for the log message according to the given DateTime
		 * 
		 * @param dateTime The DateTime when the log is created at
		 * 
		 * @return The time prefix for the log message
		*/
		static std::string createMessageTimePrefix(const DateTime& dateTime);
		/**
		 * @brief Adds source file and line information to the original message (DEBUG and TRACE severity only)
		 * 
		 * @param message The original message which has to be completed
		 * @param source The name of the source file where the log originates
		 * @param line The line number where the log originates
		 * 
		 * @return The original message completed with the source file and line information
		*/
		static std::string createDetailedMessage(const std::string& message, std::string_view source, const int line);
		/**
		 * @brief Writes the log message to the console if the corresponding flag is set
		 * 
		 * @param message The message of the log with the prefixes
		*/
		static void writeLogToConsole(std::string_view message);
		/**
		 * @brief Writes the log message with the severity and time prefixes prepended to the log file
		 * 
		 * @param message The raw log message which will be prepended with the prefixes
		 * @param dateTime The creation DateTime of the log. It determines the name of the log file and the time message prefix
		*/
		static void writeLogToFile(std::string_view message, const DateTime& dateTime);
		/**
		 * @brief Notifies the attached receivers by forwarding them the log message
		 * 
		 * @param message The raw message of the log without the prefixes
		*/
		static void notifyReceivers(std::string_view message);
		/**
		 * @brief Calculates the current DateTime
		 * 
		 * @return Returns the current DateTime
		*/
		static DateTime currentDateTime();
		/**
		 * @brief Checks whether the defined log path exists and creates it if needed
		*/
		static void checkLogPath();
		/**
		 * @brief Checks the name of the log file according to the given DateTime and file size limit
		 * 
		 * @param dateTime The DateTime of the log creation. Its date properties are used to define the name of the log file
		 * 
		 * @return The calculated name of the log file
		*/
		static std::string checkLogFile(const DateTime& dateTime);
		/**
		 * @brief Checks the indexing of the log file according to the log file size limitation
		 * 
		 * @param nameBase The base name which needs indexing (if needed due to the size limitation)
		 * @param index The currently checked index
		 * @param filename The indexed (if needed) log file name
		 * 
		 * @return The check status. True if the log file name check was unsuccessful
		*/
		static bool checkLogFileIndexing(std::string_view nameBase, int& index, std::string& filename);

		/**
		 * @brief Sets the initialization flag to false. This is used for testing purposes only
		*/
		static void uninitializeLogger();

	public:
		/**
		 * @brief Simple initialization of the Logger. Default value is applied for each optional variable
		 * 
		 * @param logPath The path to the folder where the log files have to be created
		*/
		static void init(std::string_view logPath);
		/**
		 * @brief Advanced initialization of the Logger. Optional variables are set through the parameters
		 * 
		 * @param logPath The path to the folder where the log files have to be created
		 * @param printLog The flag which indicates whether the log message should be printed on the console
		 * @param severityLimit The maximum allowed severity of the logs
		 * @param sizeLimit The maximum allowed size of the log files in bytes
		*/
		static void init(std::string_view logPath, const bool printLog, const LogSeverity severityLimit, const int sizeLimit);

		/**
		 * @brief Simple initialization of the Logger to save logs in the AppData. Default value is applied for each optional variable
		 * 
		 * @param application The name of the application the log is created for
		 * @param domain The name of the domain this application belongs to
		*/
		static void init(const std::string& application, const std::string& domain);
		/**
		 * @brief Advanced initialization of the Logger to save logs in the AppData. Optional variables are set through the parameters
		 * 
		 * @param application The name of the application the log is created for
		 * @param domain The name of the domain this application belongs to
		 * @param printLog The flag which indicates whether the log message should be printed on the console
		 * @param severityLimit The maximum allowed severity of the logs
		 * @param sizeLimit The maximum allowed size of the log files in bytes
		*/
		static void init(const std::string& application, const std::string& domain, const bool printLog, const LogSeverity severityLimit, const int sizeLimit);

		/**
		 * @brief Adds the given Receiver object to the Logger. The Logger does not take the Receiver object's ownership!
		 * 
		 * @param receiver The Receiver object to be added to the Logger
		*/
		static void addReceiver(Receiver* receiver);
		/**
		 * @brief Removes the given Receiver object from the Logger. The removed object is not deleted!
		 * 
		 * @param receiver The Receiver object to be removed from the Logger
		*/
		static void removeReceiver(Receiver* receiver);
		/**
		 * @brief Removes every attached Receiver from the Logger. The removed objects are not deleted!
		*/
		static void clearReceivers();

		/**
		 * @brief Creates a log with INFO severity
		 * 
		 * @param message The message to be logged
		*/
		static void logInfo(const std::string& message);
		/**
		 * @brief Creates a log with WARNING severity
		 * 
		 * @param message The message to be logged
		*/
		static void logWarning(const std::string& message);
		/**
		 * @brief Creates a log with ERROR severity
		 * 
		 * @param message The message to be logged
		*/
		static void logError(const std::string& message);
		/**
		 * @brief Creates a log with DEBUG severity
		 * 
		 * @param message The message to be logged
		*/
		static void logDebug(const std::string& message, std::string_view source, const int line);
		/**
		 * @brief Creates a log with TRACE severity
		 * 
		 * @param message The message to be logged
		*/
		static void logTrace(const std::string& message, std::string_view source, const int line);
	};
}