#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : EventRef(nullptr, EventData(), [](TrackMessage &&) {})
{
    // very empty indeed.
}