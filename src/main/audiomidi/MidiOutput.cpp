#include "MidiOutput.hpp"

using namespace mpc::audiomidi;

MidiOutput::MidiOutput() {}

void MidiOutput::panic() const {}

bool MidiOutput::enqueueEvent(MidiEvent e)
{
    if (queue.enqueue(std::move(e)))
    {
        return true;
    }
    if (const auto *payload = e.getSysExPayload())
    {
        payload->pool->reportFull();
    }
    return false;
}

int MidiOutput::dequeue(std::vector<MidiEvent> &buf)
{
    MidiEvent e;
    size_t idx = 0;

    while (idx < buf.size() && queue.dequeue(e))
    {
        buf[idx++] = std::move(e);
    }

    return idx;
}
