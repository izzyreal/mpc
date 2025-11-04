#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include <vector>
#include <memory>

namespace moodycamel {
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits>
    class ConcurrentQueue;
}

namespace mpc::audiomidi
{

    class MidiOutput
    {
    public:
        MidiOutput();

    private:
        using MidiEvent = client::event::ClientMidiEvent;
        using MidiEventPtr = std::shared_ptr<MidiEvent>;
        using MidiEventQueue = moodycamel::ConcurrentQueue<MidiEventPtr, moodycamel::ConcurrentQueueDefaultTraits>;

        std::shared_ptr<MidiEventQueue> queue;

    public:
        void enqueueEvent(MidiEventPtr);
        int dequeue(std::vector<MidiEventPtr> &);
        void panic();
    };
} // namespace mpc::audiomidi
