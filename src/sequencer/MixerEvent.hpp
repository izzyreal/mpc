#pragma once
#include <sequencer/Event.hpp>

namespace mpc {
	namespace sequencer {

		class MixerEvent
			: public Event
		{

		private:
			int mixerParameter{ 0 };
			int padNumber{ 0 };
			int mixerParameterValue{ 0 };

		public:
			void setParameter(int i);
			int getParameter();
			void setPadNumber(int i);
			int getPad();
			void setValue(int i);
			int getValue();

			void CopyValuesTo(std::weak_ptr<Event> dest) override;

			MixerEvent();

		};

	}
}
