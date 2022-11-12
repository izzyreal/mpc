#include "MpcMidiOutput.hpp"

using namespace mpc::audiomidi;
using namespace ctoot::midi::core;

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

void MpcMidiOutput::enqueMessageOutputA(std::shared_ptr<ctoot::midi::core::ShortMessage> msg)
{
    outputQueueA.enqueue(msg);
}

void MpcMidiOutput::enqueMessageOutputB(std::shared_ptr<ctoot::midi::core::ShortMessage> msg)
{
    outputQueueB.enqueue(msg);
}

unsigned char MpcMidiOutput::dequeueOutputA(std::vector<std::shared_ptr<ctoot::midi::core::ShortMessage>>& buf)
{
    return static_cast<unsigned char>(outputQueueA.try_dequeue_bulk(buf.begin(), buf.size()));
}

unsigned char MpcMidiOutput::dequeueOutputB(std::vector<std::shared_ptr<ctoot::midi::core::ShortMessage>>& buf)
{
    return static_cast<unsigned char>(outputQueueB.try_dequeue_bulk(buf.begin(), buf.size()));
}
