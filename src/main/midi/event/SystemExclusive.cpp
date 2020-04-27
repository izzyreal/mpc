#include <midi/event/SystemExclusive.hpp>

#include <midi/event/MidiEvent.hpp>
#include <midi/util/VariableLengthInt.hpp>

using namespace mpc::midi::event;
using namespace std;

SystemExclusiveEvent::SystemExclusiveEvent(int type, int tick, vector<char> data) : SystemExclusiveEvent(type, tick, 0, data)
{
	
}

SystemExclusiveEvent::SystemExclusiveEvent(int type, int tick, int delta, vector<char> data) : MidiEvent(tick, delta)
{
	mType = type & 255;
	if (mType != 240 && mType != 247) {
		mType = 240;
	}
	mLength = new mpc::midi::util::VariableLengthInt(data.size());
	mData = data;
}

vector<char> SystemExclusiveEvent::getData()
{
    return mData;
}

void SystemExclusiveEvent::setData(vector<char> data)
{
	mLength->setValue(data.size());
	mData = data;
}

bool SystemExclusiveEvent::requiresStatusByte(MidiEvent* prevEvent)
{
    return true;
}

void SystemExclusiveEvent::writeToOutputStream(ostream& out, bool writeType)
{
	MidiEvent::writeToOutputStream(out, writeType);
	out << mType;
	out.write(&mLength->getBytes()[0], mLength->getBytes().size());
	out.write(&mData[0], mData.size());
}

int SystemExclusiveEvent::compareTo(MidiEvent* other)
{
	if (this->mTick < other->mTick) {
		return -1;
	}
	
	if (this->mTick > other->mTick) {
		return 1;
	}
	
	if (mDelta.getValue() > other->mDelta.getValue()) {
		return -1;
	}

	if (mDelta.getValue() < other->mDelta.getValue()) {
		return 1;
	}

    if(dynamic_cast< SystemExclusiveEvent* >(other) != nullptr) {
		string curr = "";
		
		for (char c : mData) {
			curr.push_back(c);
		}
		
		auto tmp = dynamic_cast<SystemExclusiveEvent*>(other)->mData;
		
		string comp = "";
		
		for (char c : tmp) {
			comp.push_back(c);
		}

		return curr.compare(comp);
    }
    return 1;
}

int SystemExclusiveEvent::getEventSize()
{
	return 1 + mLength->getByteCount() + mData.size();
}
