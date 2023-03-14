#include "MpcMidiOutput.hpp"

using namespace mpc::audiomidi;
using namespace mpc::engine::midi;

void MpcMidiOutput::panic()
{
    auto msg = std::make_shared<ShortMessage>();

    for (int ch = 0; ch < 16; ch++)
    {
        for (int note = 0; note < 128; note++)
        {
            msg->setMessage(ShortMessage::NOTE_OFF, ch, note, 0);
            outputQueueA.enqueue(msg);
        }
    }
}

void MpcMidiOutput::enqueueMessageOutputA(std::shared_ptr<mpc::engine::midi::ShortMessage> msg)
{
    outputQueueA.enqueue(msg);
}

void MpcMidiOutput::enqueueMessageOutputB(std::shared_ptr<mpc::engine::midi::ShortMessage> msg)
{
    outputQueueB.enqueue(msg);
}

unsigned char MpcMidiOutput::dequeueOutputA(std::vector<std::shared_ptr<mpc::engine::midi::ShortMessage>>& buf)
{
    return static_cast<unsigned char>(outputQueueA.try_dequeue_bulk(buf.begin(), buf.size()));
}

unsigned char MpcMidiOutput::dequeueOutputB(std::vector<std::shared_ptr<mpc::engine::midi::ShortMessage>>& buf)
{
    return static_cast<unsigned char>(outputQueueB.try_dequeue_bulk(buf.begin(), buf.size()));
}
