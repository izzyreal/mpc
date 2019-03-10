#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData;

				class MidiChannelPrefix
					: public MetaEvent
				{

				private:
					int mChannel{};

				public:
					virtual void setChannel(int c);
					virtual int getChannel();

				public:
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out)  override;
					static std::shared_ptr<MetaEvent> parseMidiChannelPrefix(int tick, int delta, MetaEventData* info);
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

					MidiChannelPrefix(int tick, int delta, int channel);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);
				};

			}
		}
	}
}
