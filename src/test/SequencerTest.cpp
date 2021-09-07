#include <catch2/catch.hpp>

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

#include <sequencer/Sequencer.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::sequencer;
using namespace std;

SCENARIO("Can record and playback from different threads", "[sequencer]")
{
    char buffer[2][512];
    
    const int BUFFER_SIZE = 512;
    const int PROCESS_BLOCK_INTERVAL = 12; // Approximate duration of 512 frames at 44100khz
    const int AUDIO_THREAD_TIMEOUT = 4000;
    const int RECORD_DELAY = 500;
    const int INITIAL_EVENT_INSERTION_DELAY = 500;

    //                      1                   2                   3                   4                   1... <loop>
    // Quantized positions: 0  , 24 , 48 , 72 , 96 , 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360, 384
    
    // The event at tick 382 is expected to be quantized to tick 0, because the sequence is one bar long.
    // Event at tick 2 will also be quantized to tick 0. Hence of the 17 ticks below, only 16 will survive.
    vector<int> humanTickPositions{ 2, 23, 49, 70, 95, 124, 143, 167, 194, 218, 243, 264, 290, 310, 332, 361, 382};
    
    vector<int> quantizedPositions{ 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360 };
    
    GIVEN("A running sequencer in record mode and some user input")
    {
        bool mainThreadBusy = true;
        bool audioThreadBusy = true;
        
        mpc::Mpc mpc;
        mpc.init(44100, 1, 5);
        
        auto seq = mpc.getSequencer().lock();
        
        auto sequence = seq->getActiveSequence().lock();
        sequence->init(0);
        
        auto track = seq->getActiveTrack().lock();
                
        auto server = mpc.getAudioMidiServices().lock()->getAudioServer();
        server->resizeBuffers(512);
        server->setSampleRate(44100);
        
        thread audioThread([&](){
            
            Event* e;
            
            int dspCycleCounter = 0;
            
            while (dspCycleCounter++ * PROCESS_BLOCK_INTERVAL < AUDIO_THREAD_TIMEOUT &&
                   track->getEvents().size() < humanTickPositions.size())
            {
                auto eventCount = track->getEvents().size();
                server->work(nullptr, nullptr, BUFFER_SIZE, 0, 1);
                
                if (dspCycleCounter * PROCESS_BLOCK_INTERVAL < RECORD_DELAY)
                {
                    this_thread::sleep_for(chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
                    continue;
                }
                
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
            
            while (tickPos < 384 && prevTickPos <= tickPos)
            {
                for (int i = 0; i < humanTickPositions.size(); i++)
                {
                    auto hTickPos = humanTickPositions[i];
                    
                    if (tickPos >= hTickPos && tickPos < hTickPos + 24)
                    {
                        if (find(begin(recordedTickPos), end(recordedTickPos), hTickPos) == end(recordedTickPos))
                        {
                            //printf("main thread hits pad 0 at tick %i ...\n", hTickPos);
                            mpc.getActiveControls().lock()->pad(0, 127, false, hTickPos);
                            
                            this_thread::sleep_for(chrono::milliseconds(2));
                            
                            //printf("main thread releases pad 0 ...\n");
                            mpc.getReleaseControls()->simplePad(0);
                        }
                    }
                }
                
                this_thread::sleep_for(chrono::milliseconds(5));
                prevTickPos = tickPos;
                tickPos = seq->getTickPosition();
            }
        
            seq->stop();
            mainThreadBusy = false;
        });
        
        while (mainThreadBusy || audioThreadBusy)
            this_thread::sleep_for(chrono::milliseconds(10));
        
        mainThread.join();
        audioThread.join();
        
        // For - 1 explanation, see humanTickPositions comment
        REQUIRE(track->getEvents().size() == humanTickPositions.size() - 1);
    }
}
