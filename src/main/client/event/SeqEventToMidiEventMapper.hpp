#pragma once

namespace mpc::sequencer
{
    struct EventData;
}

namespace mpc::client::event
{
    struct ClientMidiEvent;
    class SeqEventToMidiEventMapper
    {
    public:
        static void mapSeqEventToMidiEvent(const sequencer::EventData &, ClientMidiEvent &);
    };
}