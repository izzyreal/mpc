#pragma once
#include <sequencer/Event.hpp>

#include <memory>

namespace mpc::sequencer
{
	class ChannelPressureEvent
		: public Event
	{

	private:
		int channelPressureValue{ 0 };

	public:
		void setAmount(int i);
		int getAmount();

		void CopyValuesTo(std::weak_ptr<Event> dest) override;

		std::string getTypeName() override { return "channel-pressure"; }

	};
}
