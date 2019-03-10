#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class EndOfTrack
					: public MetaEvent
				{

				public:
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out) override;
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

					EndOfTrack(int tick, int delta);

					void writeToFile(moduru::io::OutputStream* out, bool writeType);

				};

			}
		}
	}
}
