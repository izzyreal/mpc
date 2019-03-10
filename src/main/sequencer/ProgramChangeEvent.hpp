#pragma once
#include <sequencer/Event.hpp>

namespace mpc {
	namespace sequencer {

		class ProgramChangeEvent
			: public Event
		{

		private:
			int programChangeValue{ 0 };

		public:
			void setProgram(int i);
			int getProgram();

			void CopyValuesTo(std::weak_ptr<Event> dest) override;

			ProgramChangeEvent();

		};

	}
}
