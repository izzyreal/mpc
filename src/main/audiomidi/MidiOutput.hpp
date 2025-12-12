#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include "concurrency/BoundedMpmcQueue.hpp"

#include <vector>

namespace mpc::audiomidi
{
    class MidiOutput
    {
    public:
        MidiOutput();

    private:
        static constexpr size_t QueueCapacity = 128;
        using MidiEvent = client::event::ClientMidiEvent;
        using MidiEventQueue = concurrency::BoundedMpmcQueue<MidiEvent, QueueCapacity>;

        MidiEventQueue queue;

    public:
        void enqueueEvent(const MidiEvent &);
        int dequeue(std::vector<MidiEvent> &);
        void panic() const;
    };
} // namespace mpc::audiomidi
