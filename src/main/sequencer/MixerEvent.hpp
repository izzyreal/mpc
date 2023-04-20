#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class MixerEvent
		: public Event
	{

	private:
		int mixerParameter{ 0 };
		int padNumber{ 0 };
		int mixerParameterValue{ 0 };

	public:
		void setParameter(int i);
		int getParameter() const;
		void setPadNumber(int i);
		int getPad() const;
		void setValue(int i);
		int getValue() const;

		MixerEvent() = default;
		MixerEvent(const MixerEvent&);
		void CopyValuesTo(std::weak_ptr<Event> dest) override;

		std::string getTypeName() override { return "mixer"; }

	};
}
