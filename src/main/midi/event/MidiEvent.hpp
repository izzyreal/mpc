#pragma once

#include <midi/util/VariableLengthInt.hpp>

#include <io/OutputStream.hpp>
#include <io/InputStream.hpp>

#include <string>
#include <memory>
#include <fstream>

using namespace std;

namespace mpc::midi::event {

	class MidiEvent
	{

	public:
		int mTick{ 0 };
		mpc::midi::util::VariableLengthInt* mDelta{ nullptr };

	public:
		virtual int getTick();
		virtual int getDelta();
		virtual void setDelta(int d);

	public:
		virtual int getEventSize() = 0;

	public:
		virtual int getSize();
		virtual bool requiresStatusByte(MidiEvent* prevEvent);
		virtual void writeToOutputStream(ostream& out, bool writeType);

	private:
		static int sId;
		static int sType;
		static int sChannel;

	public:
		static shared_ptr<MidiEvent> parseEvent(int tick, int delta, moduru::io::InputStream* in);

	private:
		static bool verifyIdentifier(int id);

	public:
		virtual string toString();

		MidiEvent(int tick, int delta);
	};
}
