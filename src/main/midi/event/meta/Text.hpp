#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class Text
					: public TextualMetaEvent
				{

				public:
					Text(int tick, int delta, std::string text);

				};
			}
		}
	}
}
