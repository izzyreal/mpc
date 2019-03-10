#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData;
				
				class GenericMetaEvent
					: public MetaEvent
				{

				private:
					std::vector<char> mData{};

				public:
					int getEventSize() override;
					void writeToFile(moduru::io::OutputStream* out)  override;

				public:
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

				public:
					GenericMetaEvent(int tick, int delta, MetaEventData* info);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);

				};
			}
		}
	}
}
