#include "MidiOutput.hpp"

using namespace mpc::audiomidi;

MidiOutput::MidiOutput()
{
}

void MidiOutput::panic() const {}

void MidiOutput::enqueueEvent(const MidiEvent &e)
{
    queue.enqueue(e);
}

int MidiOutput::dequeue(std::vector<MidiEvent> &buf)
{
    MidiEvent e;
    size_t idx = 0;

    while (queue.dequeue(e) && idx < buf.size())
    {
        buf[idx++] = e;
    }

    return idx;
}
