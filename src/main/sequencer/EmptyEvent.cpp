#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : Event(
          []
          {
              return EventState();
          },
          [](NonRtSequencerMessage &&) {})
{
    // very empty indeed.
}