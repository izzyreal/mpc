#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : Event(
          []
          {
              return EventState();
          },
          [](SequencerMessage &&) {})
{
    // very empty indeed.
}