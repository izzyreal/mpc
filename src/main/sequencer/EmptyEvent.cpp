#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : Event(
          []
          {
              return std::pair{NoEventIndex, EventState()};
          },
          [](TrackEventMessage &&) {})
{
    // very empty indeed.
}