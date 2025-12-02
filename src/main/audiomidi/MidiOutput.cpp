#include "MidiOutput.hpp"

#include <concurrentqueue.h>

using namespace mpc::audiomidi;

MidiOutput::MidiOutput()
{
    queue = std::make_shared<MidiEventQueue>(100);
}

void MidiOutput::panic() const {}

void MidiOutput::enqueueEvent(const MidiEvent &e) const
{
    queue->enqueue(e);
}

int MidiOutput::dequeue(std::vector<MidiEvent> &buf) const
{
    return queue->try_dequeue_bulk(buf.begin(), buf.size());
}
