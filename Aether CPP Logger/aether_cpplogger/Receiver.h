#pragma once
#include <string_view>

namespace aether_cpplogger
{
	class Receiver
	{
	public:
		Receiver() = default;
		virtual ~Receiver() = default;

		virtual void onReceive(std::string_view message) = 0;
	};
}