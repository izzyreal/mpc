#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent() : Event([]{return performance::Event();})
{
    // very empty indeed.
}