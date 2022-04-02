#include "pch.h"
#include "CppUnitTest.h"

#include "LoggerMock.h"
#include "ReceiverMock.h"
#include "LoggerException.h"

#include <iostream>
#include <filesystem>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace aether_cpplogger_tests
{
	TEST_CLASS(LoggerTest)
	{
	private:
		const std::string testLogPath = "LoggerTest";
		const std::string testMessage = "This is a test";
		const std::string testLogFilename = "2022-3-22.log";
		aether_cpplogger::Logger::DateTime testDateTime;
		
		TEST_METHOD_INITIALIZE(Setup)
		{
			aether_cpplogger::Logger::clearReceivers();

			testDateTime.Year = 2022;
			testDateTime.Month = 3;
			testDateTime.Day = 22;
			testDateTime.Hours = 11;
			testDateTime.Minutes = 32;
			testDateTime.Seconds = 53;
		}
		TEST_METHOD(UninitializedLogTest)
		{
			try
			{
				LoggerMock::uninitializeLogger();
				aether_cpplogger::Logger::logInfo(testMessage);
			}
			catch (const aether_cpplogger::LoggerException& ex)
			{
				Assert::AreEqual("Logger is not initialized", ex.what());
				return;
			}

			Assert::Fail(L"Expected exception is was not thrown");
		}

		TEST_METHOD(LogSeverityLimitTest)
		{
			if (std::filesystem::exists(testLogPath))
			{
				std::filesystem::remove_all(testLogPath);
			}
			Assert::IsFalse(std::filesystem::exists(testLogPath), L"Initial log path directory should not exist");

			aether_cpplogger::Logger::init(testLogPath, false, aether_cpplogger::LogSeverity::INFO, 10);
			aether_cpplogger::Logger::logWarning(testMessage);
			aether_cpplogger::Logger::logError(testMessage);
			aether_cpplogger::Logger::logDebug(testMessage, __FILE__, __LINE__);
			aether_cpplogger::Logger::logTrace(testMessage, __FILE__, __LINE__);
			Assert::IsFalse(std::filesystem::exists(testLogPath), L"Log path directory should not exist");

			aether_cpplogger::Logger::logInfo(testMessage);
			Assert::IsTrue(std::filesystem::exists(testLogPath), L"Log path directory should not exist");

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(AppDataPathTest)
		{
			const std::string testApplication = "TestApp";
			const std::string testDomain = "TestDomain";
			const auto& appDataFolder = LoggerMock::createAppDataPathTest(testApplication, testDomain);
			const std::string expectedAppDataFolderEnding = "\\AppData\\Roaming\\TestDomain\\TestApp\\logs";
			const auto endsWith = appDataFolder.compare(appDataFolder.size() - expectedAppDataFolderEnding.size(), expectedAppDataFolderEnding.size(), expectedAppDataFolderEnding);
			Assert::IsTrue(endsWith == 0);
		}

		TEST_METHOD(AppDataPathTestWithoutDomain)
		{
			const std::string testApplication = "TestApp";
			const std::string testDomain = "";
			const auto& appDataFolder = LoggerMock::createAppDataPathTest(testApplication, testDomain);
			const std::string expectedAppDataFolderEnding = "\\AppData\\Roaming\\TestApp\\logs";
			const auto endsWith = appDataFolder.compare(appDataFolder.size() - expectedAppDataFolderEnding.size(), expectedAppDataFolderEnding.size(), expectedAppDataFolderEnding);
			Assert::IsTrue(endsWith == 0);
		}

		TEST_METHOD(MessageSeverityPrefixTest)
		{
			const auto& infoPrefix = LoggerMock::createMessageSeverityPrefixTest(aether_cpplogger::LogSeverity::INFO);
			Assert::AreEqual("[INFO]\t\t", infoPrefix.c_str());

			const auto& warningPrefix = LoggerMock::createMessageSeverityPrefixTest(aether_cpplogger::LogSeverity::WARNING);
			Assert::AreEqual("[WARNING]\t", warningPrefix.c_str());

			const auto& errorPrefix = LoggerMock::createMessageSeverityPrefixTest(aether_cpplogger::LogSeverity::ERROR);
			Assert::AreEqual("[ERROR]\t\t", errorPrefix.c_str());

			const auto& debugPrefix = LoggerMock::createMessageSeverityPrefixTest(aether_cpplogger::LogSeverity::DEBUG);
			Assert::AreEqual("[DEBUG]\t\t", debugPrefix.c_str());

			const auto& tracePrefix = LoggerMock::createMessageSeverityPrefixTest(aether_cpplogger::LogSeverity::TRACE);
			Assert::AreEqual("[TRACE]\t\t", tracePrefix.c_str());
		}

		TEST_METHOD(MessageTimePrefixTest)
		{
			const std::string expectedTimePrefix = "11:32:53\t\t";
			const auto& timePrefix = LoggerMock::createMessageTimePrefixTest(testDateTime);
			Assert::AreEqual(expectedTimePrefix, timePrefix);
		}

		TEST_METHOD(DateTimeTest)
		{
			const auto& currentDateTime = LoggerMock::currentDateTimeTest();

			const time_t now = time(nullptr);
			tm ltm;
			localtime_s(&ltm, &now);

			const int currentYear = 1900 + ltm.tm_year;
			const int currentMonth = 1 + ltm.tm_mon;
			const int currentDay = ltm.tm_mday;
			const int currentHours = ltm.tm_hour;
			const int currentMinutes = ltm.tm_min;
			const int currentSeconds = ltm.tm_sec;

			const std::string expectedDate = std::to_string(currentYear) + "-" +
				std::to_string(currentMonth) + "-" +
				std::to_string(currentDay);

			Assert::AreEqual(expectedDate, currentDateTime.currentDateString());

			const std::string expectedTime = std::to_string(currentHours) + ":" +
				std::to_string(currentMinutes) + ":" +
				std::to_string(currentSeconds);

			Assert::AreEqual(expectedTime, currentDateTime.currentTimeString());
		}

		TEST_METHOD(LogPathTest)
		{
			if(std::filesystem::exists(testLogPath))
			{
				std::filesystem::remove_all(testLogPath);
			}
			Assert::IsFalse(std::filesystem::exists(testLogPath), L"Log path directory should not exist");

			aether_cpplogger::Logger::init(testLogPath);
			LoggerMock::checkLogPathTest();
			Assert::IsTrue(std::filesystem::exists(testLogPath), L"Log path directory should exist");

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(LogFileWithEmptyDirectoryTest)
		{
			std::filesystem::create_directory(testLogPath);

			aether_cpplogger::Logger::init(testLogPath);
			const auto& filename = LoggerMock::checkLogFileTest(testDateTime);
			Assert::AreEqual(testLogFilename.c_str(), filename.c_str());

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(LogFileWithExistingZeroSizeFileTest)
		{
			std::filesystem::create_directory(testLogPath);

			std::ofstream testLogFile;
			testLogFile.open(testLogPath + "\\" + testLogFilename);
			testLogFile.close();

			aether_cpplogger::Logger::init(testLogPath);
			const auto& filename = LoggerMock::checkLogFileTest(testDateTime);
			Assert::AreEqual(testLogFilename.c_str(), filename.c_str());

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(LogFileWithExistingLimitReachedSizeFileTest)
		{
			std::filesystem::create_directory(testLogPath);

			std::ofstream testLogFile;
			testLogFile.open(testLogPath + "\\" + testLogFilename);
			testLogFile.seekp(10);
			testLogFile.write("", 1);
			testLogFile.close();

			const std::string expectedLogFilename = "2022-3-22_2.log";
			aether_cpplogger::Logger::init(testLogPath, false, aether_cpplogger::LogSeverity::ERROR, 9);
			const auto& filename = LoggerMock::checkLogFileTest(testDateTime);
			Assert::AreEqual(expectedLogFilename.c_str(), filename.c_str());

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(WriteToConsoleTest)
		{
			std::stringstream buffer;
			std::streambuf* sbuf = std::cout.rdbuf();
			std::cout.rdbuf(buffer.rdbuf());

			aether_cpplogger::Logger::init("", true, aether_cpplogger::LogSeverity::ERROR, 10);
			LoggerMock::writeLogToConsoleTest(testMessage);

			Assert::AreEqual(testMessage + "\n", buffer.str());

			std::cout.rdbuf(sbuf);
		}

		TEST_METHOD(WriteLogToFileWithoutDirectoryTest)
		{
			if (std::filesystem::exists(testLogPath))
			{
				std::filesystem::remove_all(testLogPath);
			}
			Assert::IsFalse(std::filesystem::exists(testLogPath), L"Log path directory should not exist");

			aether_cpplogger::Logger::init(testLogPath);
			LoggerMock::writeLogToFileTest(testMessage, testDateTime);

			Assert::IsTrue(std::filesystem::exists(testLogPath), L"Log path directory should exist");
			Assert::IsTrue(std::filesystem::exists(testLogPath + "\\" + testLogFilename), L"Log file should exist");

			std::ifstream inLogFile;
			inLogFile.open(testLogPath + "\\" + testLogFilename);
			std::string fileContent;

			inLogFile.seekg(0, std::ios::end);
			fileContent.reserve(inLogFile.tellg());
			inLogFile.seekg(0, std::ios::beg);

			fileContent.assign((std::istreambuf_iterator<char>(inLogFile)),
				std::istreambuf_iterator<char>());

			inLogFile.close();

			Assert::AreEqual(testMessage + "\n", fileContent, L"The file content is incorrect");

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(WriteLogToFileTestWithAlreadyExistingFile)
		{
			if (std::filesystem::exists(testLogPath))
			{
				std::filesystem::remove_all(testLogPath);
			}
			std::filesystem::create_directory(testLogPath);

			std::ofstream testLogFile;
			testLogFile.open(testLogPath + "\\" + testLogFilename);
			testLogFile << testMessage << std::endl;
			testLogFile.close();

			Assert::IsTrue(std::filesystem::exists(testLogPath), L"Log path directory should exist");
			Assert::IsTrue(std::filesystem::exists(testLogPath + "\\" + testLogFilename), L"Initial log file should exist");

			aether_cpplogger::Logger::init(testLogPath, true, aether_cpplogger::LogSeverity::ERROR, 1024);
			LoggerMock::writeLogToFileTest(testMessage, testDateTime);

			std::ifstream inLogFile;
			inLogFile.open(testLogPath + "\\" + testLogFilename);
			std::string fileContent;

			inLogFile.seekg(0, std::ios::end);
			fileContent.reserve(inLogFile.tellg());
			inLogFile.seekg(0, std::ios::beg);

			fileContent.assign((std::istreambuf_iterator<char>(inLogFile)),
				std::istreambuf_iterator<char>());

			inLogFile.close();

			const std::string expectedMessage = testMessage + "\n" + testMessage + "\n";
			Assert::AreEqual(expectedMessage, fileContent, L"The file content is incorrect");

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(WriteLogToFileTestWithAlreadyExistingFileWithSizeLimit)
		{
			if (std::filesystem::exists(testLogPath))
			{
				std::filesystem::remove_all(testLogPath);
			}
			std::filesystem::create_directory(testLogPath);

			std::ofstream testLogFile;
			testLogFile.open(testLogPath + "\\" + testLogFilename);
			testLogFile << testMessage << std::endl;
			testLogFile.close();

			Assert::IsTrue(std::filesystem::exists(testLogPath), L"Log path directory should exist");
			Assert::IsTrue(std::filesystem::exists(testLogPath + "\\" + testLogFilename), L"Initial log file should exist");

			aether_cpplogger::Logger::init(testLogPath, true, aether_cpplogger::LogSeverity::ERROR, 1);
			LoggerMock::writeLogToFileTest(testMessage, testDateTime);

			const std::string expectedLogFilename = "2022-3-22_2.log";
			std::ifstream inLogFile;
			inLogFile.open(testLogPath + "\\" + expectedLogFilename);
			std::string fileContent;

			inLogFile.seekg(0, std::ios::end);
			fileContent.reserve(inLogFile.tellg());
			inLogFile.seekg(0, std::ios::beg);

			fileContent.assign((std::istreambuf_iterator<char>(inLogFile)),
				std::istreambuf_iterator<char>());

			inLogFile.close();

			const std::string expectedMessage = testMessage + "\n";
			Assert::AreEqual(expectedMessage, fileContent, L"The file content is incorrect");

			std::filesystem::remove_all(testLogPath);
		}

		TEST_METHOD(NotifyReceiversTest)
		{
			auto receiverMock1 = new ReceiverMock();
			auto receiverMock2 = new ReceiverMock();
			auto receiverMock3 = new ReceiverMock();

			aether_cpplogger::Logger::addReceiver(receiverMock1);
			aether_cpplogger::Logger::addReceiver(receiverMock2);
			aether_cpplogger::Logger::addReceiver(receiverMock3);

			LoggerMock::notifyReceiversTest(testMessage);

			Assert::AreEqual(testMessage.c_str(), receiverMock1->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreEqual(testMessage.c_str(), receiverMock2->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreEqual(testMessage.c_str(), receiverMock3->testMessage().c_str(), "The message of the receiver is incorrect");

			aether_cpplogger::Logger::removeReceiver(receiverMock1);
			aether_cpplogger::Logger::removeReceiver(receiverMock2);
			aether_cpplogger::Logger::removeReceiver(receiverMock3);

			LoggerMock::notifyReceiversTest(testMessage + "_2");

			Assert::AreEqual(testMessage.c_str(), receiverMock1->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreEqual(testMessage.c_str(), receiverMock2->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreEqual(testMessage.c_str(), receiverMock3->testMessage().c_str(), "The message of the receiver is incorrect");
		}

		TEST_METHOD(NotifyReceiversClearTest)
		{
			auto receiverMock1 = new ReceiverMock();
			auto receiverMock2 = new ReceiverMock();
			auto receiverMock3 = new ReceiverMock();

			aether_cpplogger::Logger::addReceiver(receiverMock1);
			aether_cpplogger::Logger::addReceiver(receiverMock2);
			aether_cpplogger::Logger::addReceiver(receiverMock3);

			aether_cpplogger::Logger::clearReceivers();

			LoggerMock::notifyReceiversTest(testMessage);

			Assert::AreNotEqual(testMessage.c_str(), receiverMock1->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreNotEqual(testMessage.c_str(), receiverMock2->testMessage().c_str(), "The message of the receiver is incorrect");
			Assert::AreNotEqual(testMessage.c_str(), receiverMock3->testMessage().c_str(), "The message of the receiver is incorrect");
		}
	};
}
