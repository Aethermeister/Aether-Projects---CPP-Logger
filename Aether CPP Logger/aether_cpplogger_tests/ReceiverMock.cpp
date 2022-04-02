#include "pch.h"
#include "ReceiverMock.h"

namespace aether_cpplogger_tests
{
	void ReceiverMock::onReceive(std::string_view message)
	{
		m_testMessage = message;
	}
	const std::string& ReceiverMock::testMessage() const
	{
		return m_testMessage;
	}
}