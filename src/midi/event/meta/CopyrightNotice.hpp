#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

#include <string>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class CopyrightNotice
					: public TextualMetaEvent
				{

				
				public:
					void setNotice(std::string t);
					std::string getNotice();

					CopyrightNotice(int tick, int delta, std::string text);
				};
			}
		}
	}
}
