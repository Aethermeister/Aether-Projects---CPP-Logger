#include "LoggerException.h"

namespace aether_cpplogger
{
	LoggerException::LoggerException(const std::string& message) : std::exception(message.c_str())
	{
	}
}