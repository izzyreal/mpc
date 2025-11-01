#include "MidiOutput.hpp"

using namespace mpc::audiomidi;

void MidiOutput::panic() {}

void MidiOutput::enqueueEvent(MidiEventPtr e)
{
    queue.enqueue(e);
}

int MidiOutput::dequeue(std::vector<MidiEventPtr> &buf)
{
    return queue.try_dequeue_bulk(buf.begin(), buf.size());
}
