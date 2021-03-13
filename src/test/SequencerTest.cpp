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
        mpc::Mpc mpc;
        mpc.init(44100, 1, 5);

        auto seq = mpc.getSequencer().lock();
        auto& recBuf = seq->getRecordBuffer();

        vector<Event*> events;
        Event* e;
        
        thread mainThread([&](){
            printf("main thread records event\n");
            auto event = new NoteEvent();
            recBuf.record(event);
        });
        
        thread audioThread([&](){
            
            int dspCycleCounter = 0;
            
            while (dspCycleCounter++ < 4)
            {
                printf("audio thread reads events\n");
                
                int eventCounter = 0;
                
                while (recBuf.get(e) == true)
                {
                    events.push_back(e);
                    eventCounter++;
                }
                
                printf("%i events found in DSP cycle %i\n", eventCounter, dspCycleCounter);
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });
                        
        mainThread.join();
        audioThread.join();
        
        REQUIRE(events.size() == 1);
        REQUIRE(recBuf.get(e) == false);
    }
}
