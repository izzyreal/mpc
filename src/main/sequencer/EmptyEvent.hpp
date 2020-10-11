#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class EmptyEvent
		: public Event
	{

	public:
		EmptyEvent();
		std::string getTypeName() override { return "empty"; }

	};
}
