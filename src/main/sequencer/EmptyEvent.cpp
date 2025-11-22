#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : Event(
          []
          {
              return EventState();
          },
          [](TrackEventMessage &&) {})
{
    // very empty indeed.
}