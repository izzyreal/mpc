#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class InstrumentName
					: public TextualMetaEvent
				{

				public:
					void setName(std::string name);
					std::string getName();

					InstrumentName(int tick, int delta, std::string name);
				};

			}
		}
	}
}
