#pragma once
#include <exception>
#include <string>

namespace aether_cpplogger
{
	/**
	 * @brief Custom exception class to handle Logger related errors
	*/
	class LoggerException : public std::exception
	{
	public:
		explicit LoggerException(const std::string& message);
	};
}