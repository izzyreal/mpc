#pragma once

#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class SequencerSpecificEvent
					: public MetaEvent
				{
				private:
					std::vector<char> mData{};

				public:
					virtual void setData(std::vector<char> data);
					virtual std::vector<char> getData();

				public:
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out)  override;
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

					SequencerSpecificEvent(int tick, int delta, std::vector<char> data);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);
				};
			}
		}
	}
}
