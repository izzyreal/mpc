#pragma once

#include <midi/event/MidiEvent.hpp>

#include <unordered_map>

namespace mpc::midi::event {

	class ChannelEvent
		: public MidiEvent
	{

	public:
		int mType{ 0 };
		int mChannel{ 0 };
		int mValue1{ 0 };
		int mValue2{ 0 };

	private:
		static std::unordered_map<int, int> mOrderMap;

	public:
		virtual int getType();
		virtual void setChannel(int c);
		virtual int getChannel();

	public:
		int getEventSize() override;

	public:
		virtual int compareTo(MidiEvent* other);
		virtual bool requiresStatusByte(MidiEvent* prevEvent) override;
		virtual void writeToOutputStream(ostream& out, bool writeType) override;
		static std::shared_ptr<ChannelEvent> parseChannelEvent(int tick, int delta, int type, int channel, moduru::io::InputStream* in);

	private:
		static void buildOrderMap();

	public:
		static const int NOTE_OFF{ 8 };
		static const int NOTE_ON{ 9 };
		static const int NOTE_AFTERTOUCH{ 10 };
		static const int CONTROLLER{ 11 };
		static const int PROGRAM_CHANGE{ 12 };
		static const int CHANNEL_AFTERTOUCH{ 13 };
		static const int PITCH_BEND{ 14 };

	public:
		ChannelEvent(int tick, int type, int channel, int param1, int param2);
		ChannelEvent(int tick, int delta, int type, int channel, int param1, int param2);

	};
}
