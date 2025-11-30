#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent() : Event(nullptr, [](SequenceMessage){})
{
    // very empty indeed.
}