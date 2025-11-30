#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent() : Event(nullptr, [](TrackMessage&&){})
{
    // very empty indeed.
}