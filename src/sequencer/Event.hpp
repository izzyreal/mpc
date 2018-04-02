#pragma once
#include <observer/Observable.hpp>

namespace ctoot {
	namespace midi {
		namespace core {
			class ShortMessage;
		}
	}
}

namespace mpc {
	namespace sequencer {

		class Event
			: public moduru::observer::Observable

		{

		public:
			int tick{ 0 };
			int track{ 0 };

		private:
			ctoot::midi::core::ShortMessage* shortMessage{ nullptr };

		public:
			void setTick(int relativeTick);
			int getTick();
			int getTrack();
			void setTrack(int i);
			virtual ctoot::midi::core::ShortMessage* getShortMessage();

			virtual void CopyValuesTo(std::weak_ptr<Event> dest);

		public:
			Event();
			virtual ~Event();

		};

	}
}
