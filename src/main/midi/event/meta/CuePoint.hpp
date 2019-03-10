#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class CuePoint
					: public TextualMetaEvent
				{

				public:
					void setCue(std::string name);
					std::string getCue();

					CuePoint(int tick, int delta, std::string marker);
				};

			}
		}
	}
}
