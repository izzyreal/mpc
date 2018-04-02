#pragma once
#include <midi/event/MidiEvent.hpp>

#include <midi/util/VariableLengthInt.hpp>
#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {

			class SystemExclusiveEvent
				: public MidiEvent
			{

			private:
				int mType{};
				mpc::midi::util::VariableLengthInt* mLength{};
				std::vector<char> mData{};

			public:
				virtual std::vector<char> getData();
				virtual void setData(std::vector<char> data);
				bool requiresStatusByte(MidiEvent* prevEvent) override;
				void writeToFile(moduru::io::OutputStream* out, bool writeType) override;
				virtual int compareTo(MidiEvent* other);

			public:
				int getEventSize() override;

			public:
				SystemExclusiveEvent(int type, int tick, std::vector<char> data);
				SystemExclusiveEvent(int type, int tick, int delta, std::vector<char> data);

			};

		}
	}
}
