#include <midi/event/meta/TimeSignatureEvent.hpp>

#include <midi/event/MidiEvent.hpp>
#include <midi/event/meta/GenericMetaEvent.hpp>
#include <midi/event/meta/MetaEventData.hpp>
#include <midi/event/meta/MetaEvent.hpp>
#include <midi/util/VariableLengthInt.hpp>

#include <cmath>

using namespace mpc::midi::event::meta;
using namespace std;

TimeSignature::TimeSignature()
    : TimeSignature(0, 0, 4, 4, DEFAULT_METER, DEFAULT_DIVISION)
{
}

TimeSignature::TimeSignature(int tick, int delta, int num, int den, int meter, int div)
    : MetaEvent(tick, delta, MetaEvent::TIME_SIGNATURE, mpc::midi::util::VariableLengthInt(4))
{
	setTimeSignature(num, den, meter, div);
}

const int TimeSignature::METER_EIGHTH;
const int TimeSignature::METER_QUARTER;
const int TimeSignature::METER_HALF;
const int TimeSignature::METER_WHOLE;
const int TimeSignature::DEFAULT_METER;
const int TimeSignature::DEFAULT_DIVISION;

void TimeSignature::setTimeSignature(int num, int den, int meter, int div)
{
    mNumerator = num;
    mDenominator = log2(den);
    mMeter = meter;
    mDivision = div;
}

int TimeSignature::getNumerator()
{
    return mNumerator;
}

int TimeSignature::getDenominatorValue()
{
    return mDenominator;
}

int TimeSignature::getRealDenominator()
{
    return static_cast< int >(pow(2, mDenominator));
}

int TimeSignature::getMeter()
{
    return mMeter;
}

int TimeSignature::getDivision()
{
    return mDivision;
}

int TimeSignature::getEventSize()
{
    return 7;
}

void TimeSignature::writeToOutputStream(ostream& out) 
{
    MetaEvent::writeToOutputStream(out);
    out << (char) 4;
    out << (char) mNumerator;
    out << (char) mDenominator;
    out << (char) mMeter;
    out << (char) mDivision;
}

void TimeSignature::writeToOutputStream(ostream& out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

shared_ptr<MetaEvent> TimeSignature::parseTimeSignature(int tick, int delta, MetaEventData* info)
{
	if (info->length.getValue() != 4) {
		return make_shared<GenericMetaEvent>(tick, delta, info);
	}
	int num = info->data[0];
	int den = info->data[1];
	int met = info->data[2];
	int fps = info->data[3];
	den = (int)(pow(2, den));
	return make_shared<TimeSignature>(tick, delta, num, den, met, fps);
}

int TimeSignature::log2(int den)
{
	switch (den) {
	case 2:
		return 1;
	case 4:
		return 2;
	case 8:
		return 3;
	case 16:
		return 4;
	case 32:
		return 5;
	}

	return 0;
}

string TimeSignature::toString()
{
	return MetaEvent::toString() + " " + to_string(mNumerator) + "/" + to_string(getRealDenominator());
}

int TimeSignature::compareTo(mpc::midi::event::MidiEvent* other)
{
    if(mTick != other->getTick()) {
        return mTick < other->getTick() ? -1 : 1;
    }
    if(mDelta.getValue() != other->getDelta()) {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if(!(dynamic_cast< TimeSignature* >(other) != nullptr)) {
        return 1;
    }
    auto o = dynamic_cast< TimeSignature* >(other);
    if(mNumerator != o->mNumerator) {
        return mNumerator < o->mNumerator ? -1 : 1;
    }
    if(mDenominator != o->mDenominator) {
        return mDenominator < o->mDenominator ? -1 : 1;
    }
    return 0;
}
