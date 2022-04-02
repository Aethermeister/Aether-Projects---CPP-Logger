#pragma once
#include "..\aether_cpplogger\Receiver.h"

namespace aether_cpplogger_tests
{
	class ReceiverMock : public aether_cpplogger::Receiver
	{
	private:
		std::string m_testMessage = "This is not a test";

	public:
		void onReceive(std::string_view message) override;

		const std::string& testMessage() const;
	};
}