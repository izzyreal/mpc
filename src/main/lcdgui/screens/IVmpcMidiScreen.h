#pragma once

#include "nvram/MidiControlPersistence.hpp"

#include <memory>
#include <cstdint>

namespace mpc::lcdgui::screens {
    class IVmpcMidiScreen {
        public:
            virtual void setLearnCandidate(const bool isNote, const int8_t channelIndex, const int8_t number, const int8_t value) = 0;
            virtual bool isLearning() = 0;
            virtual std::shared_ptr<mpc::nvram::MidiControlPreset> getActivePreset() = 0;
    };
} // namespace mpc::lcdgui::screens
