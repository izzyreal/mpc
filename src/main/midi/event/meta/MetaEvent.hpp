#pragma once
#include <midi/event/MidiEvent.hpp>

#include <midi/util/VariableLengthInt.hpp>

#include <sstream>
#include <ostream>

namespace mpc::midi::event::meta {

	class MetaEvent
		: public mpc::midi::event::MidiEvent
	{

	protected:
		int mType = 0;
		mpc::midi::util::VariableLengthInt mLength;

	public:
		virtual void writeToOutputStream(ostream& output);
		void writeToOutputStream(ostream& out, bool writeType) override;

	public:
		static std::shared_ptr<MetaEvent> parseMetaEvent(int tick, int delta, stringstream& in);
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
		MetaEvent(int tick, int delta, int type);

	};
}
