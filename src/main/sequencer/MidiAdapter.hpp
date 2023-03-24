//#pragma once
//
//#include <memory>
//
//namespace mpc::engine::midi {
//    class MidiMessage;
//
//    class ShortMessage;
//}
//
//namespace mpc::sequencer {
//
//    class Event;
//
//    class OldNoteEvent;
//
//    class MidiAdapter
//    {
//
//    private:
//        std::shared_ptr<mpc::engine::midi::ShortMessage> message;
//
//    public:
//        void process(std::weak_ptr<Event> event, int channel, int newVelo);
//
//    private:
//        void convert(OldNoteEvent *event, int channel, int newVelo);
//
//    public:
//        std::weak_ptr<mpc::engine::midi::ShortMessage> get();
//
//        MidiAdapter();
//
//        ~MidiAdapter();
//
//    };
//
//}
