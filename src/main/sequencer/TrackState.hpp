#pragma once

#include "sequencer/EventState.hpp"
#include "sequencer/BusType.hpp"

namespace mpc::sequencer
{
    struct TrackState
    {
        EventState *head;
        EventIndex playEventIndex;
        BusType busType;
        bool on;
        uint8_t velocityRatio;
        uint8_t programChange;
        uint8_t device;
        bool used;

        TrackState()
        {
            initializeDefaults();
        }

        void initializeDefaults()
        {
            head = nullptr;
            playEventIndex = EventIndex(0);
            busType = BusType::DRUM1;
            on = true;
            velocityRatio = 100;
            programChange = 0;
            device = 0;
            used = false;
        }
    };
} // namespace mpc::sequencer
