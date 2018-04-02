#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

#include <string>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class TextualMetaEvent
					: public MetaEvent
				{

				public:
					std::string mText{ "" };

				public:
					virtual void setText(std::string t);
					virtual std::string getText();
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out)  override;
					int compareTo(mpc::midi::event::MidiEvent* other);
					std::string toString();

				public:
					TextualMetaEvent(int tick, int delta, int type, std::string text);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);
				};

			}
		}
	}
}
