#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include <concurrentqueue.h>

#include <vector>
#include <memory>

namespace mpc::audiomidi
{

    class MidiOutput
    {

    private:
        using MidiEvent = client::event::ClientMidiEvent;
        using MidiEventPtr = std::shared_ptr<MidiEvent>;
        using MidiEventQueue = moodycamel::ConcurrentQueue<MidiEventPtr>;

        MidiEventQueue queue = MidiEventQueue(100);

    public:
        void enqueueEvent(MidiEventPtr);
        int dequeue(std::vector<MidiEventPtr> &);
        void panic();
    };
} // namespace mpc::audiomidi
