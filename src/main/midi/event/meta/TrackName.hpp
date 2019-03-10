#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

#include <string>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class TrackName
					: public TextualMetaEvent
				{

				public:
					void setName(std::string name);
					std::string getTrackName();

					TrackName(int tick, int delta, std::string name);
				};

			}
		}
	}
}
