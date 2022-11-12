#include <midi/event/MidiEvent.hpp>

#include <midi/event/ChannelEvent.hpp>
#include <midi/event/SystemExclusive.hpp>
#include <midi/event/meta/MetaEvent.hpp>

#include <Logger.hpp>

using namespace mpc::midi::event;

MidiEvent::MidiEvent(int tick, int delta)
{
	mTick = tick;
	mDelta = mpc::midi::util::VariableLengthInt(delta);
}

int MidiEvent::getTick()
{
    return mTick;
}

int MidiEvent::getDelta()
{
    return mDelta.getValue();
}

void MidiEvent::setDelta(int d)
{
    mDelta.setValue(d);
}

int MidiEvent::getSize()
{
    return getEventSize() + mDelta.getByteCount();
}

bool MidiEvent::requiresStatusByte(MidiEvent* prevEvent)
{
	if (prevEvent == nullptr) {
		return true;
	}

	if (dynamic_cast<mpc::midi::event::meta::MetaEvent*>(this) != nullptr) {
		return true;
	}

	if (typeid(this) == typeid(prevEvent)) {
		return false;
	}

	return true;
}

void MidiEvent::writeToOutputStream(std::ostream& out, bool writeType)
{
	auto bytes = mDelta.getBytes();
	out.write(&bytes[0], bytes.size());
}

int MidiEvent::sId = -1;
int MidiEvent::sType = -1;
int MidiEvent::sChannel = -1;

std::shared_ptr<MidiEvent> MidiEvent::parseEvent(int tick, int delta, std::stringstream& in)
{
	auto reset = false;

	auto pos = in.tellg();

	auto id = (int) (in.get() & 0xFF);
	
	if (!verifyIdentifier(id)) {
		in.seekg(pos);
		reset = true;
	}

	if (sType >= 8 && sType <= 14) {
		return ChannelEvent::parseChannelEvent(tick, delta, sType, sChannel, in);
	}
	else if (sId == 255) {
		return meta::MetaEvent::parseMetaEvent(tick, delta, in);
	}
	else if (sId == 240 || sId == 247) {
		auto size = mpc::midi::util::VariableLengthInt(in);
		auto data = std::vector<char>(size.getValue());
		in.read(&data[0], data.size());
		return std::make_shared<SystemExclusiveEvent>(sId, tick, delta, data);
	}
	else {
		std::string error = "Unable to handle status byte, skipping: " + std::to_string(sId);
		if (reset) {
			in.ignore(1);
		}
	}

	return {};
}

bool MidiEvent::verifyIdentifier(int id)
{
	sId = id;
	auto type = id >> 4;
	auto channel = id & 15;
	if (type >= 8 && type <= 14) {
		sId = id;
		sType = type;
		sChannel = channel;
	}
	else if (id == 255) {
		sId = id;
		sType = -1;
		sChannel = -1;
	}
	else if (type == 15) {
		sId = id;
		sType = type;
		sChannel = -1;
	}
	else {
		return false;
	}
	return true;
}

std::string MidiEvent::toString()
{
	return std::to_string(mTick) + " (" + std::to_string(mDelta.getValue()) + "): " + typeid(this).name();
}
