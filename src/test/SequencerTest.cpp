#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::sequencer;
using namespace std;

SCENARIO("Can record and playback from different threads", "[sequencer]")
{
    GIVEN("A RecordBuffer")
    {
        mpc::Mpc mpc;
        mpc.init(44100, 1, 5);
        
        auto seq = mpc.getSequencer().lock();
        auto& recBuf = seq->getRecordBuffer();
        
        auto event = new NoteEvent();
        recBuf.record(event);
        
        vector<Event*> events;
        Event* e;
        
        while (recBuf.get(e) == true)
            events.push_back(e);
        
        REQUIRE(events.size() == 1);
        REQUIRE(recBuf.get(e) == false);
    }
    
    GIVEN("A RecordBuffer and 2 threads")
    {
        bool mainThreadBusy = true;
        bool audioThreadBusy = true;
        
        mpc::Mpc mpc;
        mpc.init(44100, 1, 5);
        
        const int PROCESS_BLOCK_INTERVAL = 17;
        const int AUDIO_THREAD_TIMEOUT = 10000;
        
        const int INITIAL_EVENT_INSERTION_DELAY = 500;
        const int EVENT_INSERTION_INTERVAL = 3;

        auto seq = mpc.getSequencer().lock();
        auto& recBuf = seq->getRecordBuffer();
        
        vector<Event*> eventsToBuffer;
        
        for (int i = 0; i < 127; i++)
        {
            for (int i = 0; i < 1; i++)
            {
                auto noteEvent = new NoteEvent();
                noteEvent->setNote(i);
                noteEvent->setTrack(i);
                eventsToBuffer.push_back(noteEvent);
            }
        }
        
        vector<Event*> eventsToRecord;
        Event* e;
        
        thread audioThread([&](){
            
            int dspCycleCounter = 0;
            
            while (dspCycleCounter++ * PROCESS_BLOCK_INTERVAL < AUDIO_THREAD_TIMEOUT &&
                   eventsToRecord.size() != eventsToBuffer.size())
            {
                int eventCounter = 0;
                
                while (recBuf.get(e) == true)
                {
                    eventsToRecord.push_back(e);
                    printf("audio thread read event %i ...\n", (int) eventsToRecord.size());
                    eventCounter++;
                }
                
                printf("%i events found in DSP cycle %i\n", eventCounter, dspCycleCounter);
                this_thread::sleep_for(chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
            }
            
            audioThreadBusy = false;
        });
                
        thread mainThread([&](){
            
            int initialDelayCounter = 0;
            
            while (initialDelayCounter++ * 10 < INITIAL_EVENT_INSERTION_DELAY)
                this_thread::sleep_for(chrono::milliseconds(10));
            
            for (int i = 0; i < eventsToBuffer.size(); i++)
            {
                printf("main thread records event %i...\n", i);
                recBuf.record(eventsToBuffer[i]);
                this_thread::sleep_for(chrono::milliseconds(EVENT_INSERTION_INTERVAL));
            }
            
            mainThreadBusy = false;
        });
                
        while (mainThreadBusy || audioThreadBusy)
            this_thread::sleep_for(chrono::milliseconds(10));
        
        mainThread.join();
        audioThread.join();
        
        REQUIRE(eventsToRecord.size() == eventsToBuffer.size());
        REQUIRE(recBuf.get(e) == false);
    }
}
