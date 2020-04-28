#include <midi/event/meta/Tempo.hpp>

#include <midi/event/MidiEvent.hpp>
#include <midi/event/meta/GenericMetaEvent.hpp>
#include <midi/event/meta/MetaEventData.hpp>
#include <midi/event/meta/MetaEvent.hpp>
#include <midi/util/MidiUtil.hpp>
#include <midi/util/VariableLengthInt.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

Tempo::Tempo() : Tempo(0, 0, DEFAULT_MPQN)
{
}

Tempo::Tempo(int tick, int delta, int mpqn)
	: MetaEvent(tick, delta, MetaEvent::TEMPO)
{
	setMpqn(mpqn);
	mLength = mpc::midi::util::VariableLengthInt(3);
}

float Tempo::DEFAULT_BPM = 120.0f;
const int Tempo::DEFAULT_MPQN;

int Tempo::getMpqn()
{
    return mMPQN;
}

float Tempo::getBpm()
{
    return mBPM;
}

void Tempo::setMpqn(int m)
{
    mMPQN = m;
    mBPM = 6.0E7f / mMPQN;
}

void Tempo::setBpm(float b)
{
    mBPM = b;
    mMPQN = static_cast< int >(60000000 / mBPM);
}

int Tempo::getEventSize()
{
    return 6;
}

void Tempo::writeToOutputStream(ostream& out) 
{
    MetaEvent::writeToOutputStream(out);
	out << (char)3;
	auto mpqn = mpc::midi::util::MidiUtil::intToBytes(mMPQN, 3);
	out.write(&mpqn[0], mpqn.size());
}

void Tempo::writeToOutputStream(ostream& out, bool writeType)
{
	MetaEvent::writeToOutputStream(out, writeType);
}

shared_ptr<MetaEvent> Tempo::parseTempo(int tick, int delta, MetaEventData* info)
{
	if (info->length.getValue() != 3) {
		return make_shared<GenericMetaEvent>(tick, delta, info);
	}
	auto mpqn = mpc::midi::util::MidiUtil::bytesToInt(info->data, 0, 3);
	return make_shared<Tempo>(tick, delta, mpqn);
}

int Tempo::compareTo(mpc::midi::event::MidiEvent* other)
{
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta.getValue() != other->getDelta()) {
		return mDelta.getValue() < other->getDelta() ? 1 : -1;
	}
	if (!(dynamic_cast<Tempo*>(other) != nullptr)) {
		return 1;
	}
	auto o = dynamic_cast<Tempo*>(other);
	if (mMPQN != o->mMPQN) {
		return mMPQN < o->mMPQN ? -1 : 1;
	}
	return 0;
}
