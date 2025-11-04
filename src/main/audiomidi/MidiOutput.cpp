#include "MidiOutput.hpp"

#include <concurrentqueue.h>

using namespace mpc::audiomidi;

MidiOutput::MidiOutput()
{
    queue = std::make_shared<MidiEventQueue>(100);
}

void MidiOutput::panic() {}

void MidiOutput::enqueueEvent(MidiEventPtr e)
{
    queue->enqueue(e);
}

int MidiOutput::dequeue(std::vector<MidiEventPtr> &buf)
{
    return queue->try_dequeue_bulk(buf.begin(), buf.size());
}
