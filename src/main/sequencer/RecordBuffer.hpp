#pragma once

#include <collections/readerwriterqueue.h>

/**
 A Sequencer has one RecordBuffer.
 
 The RecordBuffer mediates writes into the Track.events containers of all tracks of all sequences.
 
 The need for this mediation arises from writes into Track.events that are the result of keyboard or mouse input events.
 These are typically handled by a main/message/UI thread. Reads, however, are performed by the audio/DSP thread.
 
 We use a mutex-free ring-buffer to accumulate write requests. Every DSP cycle the buffered requests are mapped to actual writes into Track.events and the buffer is flushed.
 */

namespace mpc::sequencer
{

class Event;

class RecordBuffer
{
private:
    const int BUFFER_SIZE = 10;
    
    moodycamel::ReaderWriterQueue<Event*> queue = moodycamel::ReaderWriterQueue<Event*>(BUFFER_SIZE);
    
public:
    void record(Event*);
    
    bool get(Event*);
    
};
}
