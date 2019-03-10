#pragma once
#include <midi/event/MidiEvent.hpp>

#include <midi/util/VariableLengthInt.hpp>

#include <io/InputStream.hpp>
#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEvent
					: public mpc::midi::event::MidiEvent
				{

				public:
					int mType{};
					mpc::midi::util::VariableLengthInt* mLength{};
				
				public:
					int getEventSize() = 0;

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType) override;

				public:
					virtual void writeToFile(moduru::io::OutputStream* out);

				public:
					static std::shared_ptr<MetaEvent> parseMetaEvent(int tick, int delta, moduru::io::InputStream* in);
					static const int SEQUENCE_NUMBER{ 0 };
					static const int TEXT_EVENT{ 1 };
					static const int COPYRIGHT_NOTICE{ 2 };
					static const int TRACK_NAME{ 3 };
					static const int INSTRUMENT_NAME{ 4 };
					static const int LYRICS{ 5 };
					static const int MARKER{ 6 };
					static const int CUE_POINT{ 7 };
					static const int MIDI_CHANNEL_PREFIX{ 32 };
					static const int END_OF_TRACK{ 47 };
					static const int TEMPO{ 81 };
					static const int SMPTE_OFFSET{ 84 };
					static const int TIME_SIGNATURE{ 88 };
					static const int KEY_SIGNATURE{ 89 };
					static const int SEQUENCER_SPECIFIC{ 127 };

				public:
					MetaEvent(int tick, int delta, int type, mpc::midi::util::VariableLengthInt* length);

				private:
					//friend class MetaEventData;
				};

			}
		}
	}
}
