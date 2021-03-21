#include <catch2/catch.hpp>

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

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
        //                      1                   2                   3                   4                   1... <loop>
        // Quantized positions: 0  , 24 , 48 , 72 , 96 , 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360, 384
        
        vector<int> humanTickPositions{ 2, 23, 49, 70, 95, 124, 143, 167, 194, 218, 243, 264, 290, 310, 332, 361, 382};
        vector<int> quantizedPositions{ 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360 };
        
        const int BUFFER_SIZE = 512;
        const int PROCESS_BLOCK_INTERVAL = 170;
        const int AUDIO_THREAD_TIMEOUT = 20000;
        const int RECORD_DELAY = 500;
        
        const int INITIAL_EVENT_INSERTION_DELAY = 500;
        
        auto seq = mpc.getSequencer().lock();
        
        auto sequence = seq->getActiveSequence().lock();
        sequence->init(0);
        
        auto& recBuf = seq->getRecordBuffer();
        
        vector<Event*> eventsToRecord;
        
        auto server = mpc.getAudioMidiServices().lock()->getAudioServer();
        server->setRealTime(false);
        
        thread audioThread([&](){
            
            Event* e;
            
            int dspCycleCounter = 0;
            
            while (dspCycleCounter++ * PROCESS_BLOCK_INTERVAL < AUDIO_THREAD_TIMEOUT &&
                   eventsToRecord.size() < humanTickPositions.size())
            {
                server->work(BUFFER_SIZE);
                
                if (dspCycleCounter * PROCESS_BLOCK_INTERVAL < RECORD_DELAY)
                {
                    this_thread::sleep_for(chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
                    continue;
                }
                
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
            
            int tickPos = seq->getTickPosition();
            
            if (!seq->isRecordingOrOverdubbing())
                seq->recFromStart();
            
            vector<int> recordedTickPos;
            int prevTickPos = -1;
            while (tickPos < 384 && prevTickPos < tickPos)
            {
                for (int i = 0; i < humanTickPositions.size(); i++)
                {
                    auto hTickPos = humanTickPositions[i];
                    
                    if (tickPos >= hTickPos && tickPos < hTickPos + 24)
                    {
                        if (find(begin(recordedTickPos), end(recordedTickPos), hTickPos) == end(recordedTickPos))
                        {
                            auto noteEvent = new NoteEvent();
                            noteEvent->setTick(hTickPos);
                            recBuf.record(noteEvent);
                        
                            recordedTickPos.push_back(hTickPos);
                        
                            printf("main thread records event %i at tick %i ...\n", i, hTickPos);
                        }
                    }
                }
                
                this_thread::sleep_for(chrono::milliseconds(5));
                prevTickPos = tickPos;
                tickPos = seq->getTickPosition();
            }
                        
            mainThreadBusy = false;
        });
        
        while (mainThreadBusy || audioThreadBusy)
            this_thread::sleep_for(chrono::milliseconds(10));
        
        mainThread.join();
        audioThread.join();
        
        REQUIRE(eventsToRecord.size() == humanTickPositions.size());
        
        Event* e;
        REQUIRE(recBuf.get(e) == false);
    }
}
