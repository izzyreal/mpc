#include "RecordBuffer.hpp"

using namespace mpc::sequencer;
using namespace std;

void RecordBuffer::record(Event* e)
{
    queue.enqueue(e);
}

bool RecordBuffer::get(Event* e)
{
    return queue.try_dequeue(e);
}
