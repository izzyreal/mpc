#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include <vector>
#include <memory>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

namespace mpc::audiomidi
{

    class MidiOutput
    {
    public:
        MidiOutput();

    private:
        using MidiEvent = client::event::ClientMidiEvent;
        using MidiEventQueue = moodycamel::ConcurrentQueue<
            MidiEvent, moodycamel::ConcurrentQueueDefaultTraits>;

        std::shared_ptr<MidiEventQueue> queue;

    public:
        void enqueueEvent(const MidiEvent &) const;
        int dequeue(std::vector<MidiEvent> &) const;
        void panic() const;
    };
} // namespace mpc::audiomidi
