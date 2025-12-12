#pragma once

#include "sequencer/EventData.hpp"
#include "sequencer/BusType.hpp"

namespace mpc::sequencer
{
    struct TrackState
    {
        EventData *eventsHead;
        EventIndex playEventIndex;

        std::string name;
        BusType busType;
        bool on;
        uint8_t velocityRatio;
        uint8_t programChange;
        uint8_t deviceIndex;
        bool used;
        bool transmitProgramChangesEnabled = true;

        TrackState()
        {
            name.reserve(Mpc2000XlSpecs::MAX_TRACK_NAME_LENGTH);
            initializeDefaults();
        }

        void initializeDefaults()
        {
            eventsHead = nullptr;
            playEventIndex = EventIndex(0);
            name = "(Unused)";
            used = false;
            on = true;

            busType = BusType::DRUM1;
            velocityRatio = 100;
            programChange = 0;
            deviceIndex = 0;
        }
    };
} // namespace mpc::sequencer
