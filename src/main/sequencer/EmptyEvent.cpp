#include "sequencer/EmptyEvent.hpp"

using namespace mpc::sequencer;

EmptyEvent::EmptyEvent()
    : Event(
          []
          {
              return sequencer::EventState();
          })
{
    // very empty indeed.
}