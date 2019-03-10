#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData;

				class SequenceNumber
					: public MetaEvent
				{

				private:
					int mNumber{};

				public:
					int getMostSignificantBits();
					int getLeastSignificantBits();
					int getSequenceNumber();
					void writeToFile(moduru::io::OutputStream* out)  override;
					static std::shared_ptr<MetaEvent> parseSequenceNumber(int tick, int delta, MetaEventData* info);

				public:
					int getEventSize() override;

				public:
					int compareTo(mpc::midi::event::MidiEvent* other);

					SequenceNumber(int tick, int delta, int number);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);
				};

			}
		}
	}
}
