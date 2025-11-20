#pragma once

#include "performance/Sequence.hpp"
#include "performance/PerformanceManager.hpp"

#include <memory>

namespace mpc::sequencer
{
    class Event;
}

namespace mpc::performance
{
    std::shared_ptr<sequencer::Event> mapPerformanceEventToSequencerEvent(std::shared_ptr<PerformanceManager>, const performance::Event &event);
}