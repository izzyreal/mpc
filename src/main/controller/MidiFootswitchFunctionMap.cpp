#include "controller/MidiFootswitchFunctionMap.hpp"

namespace mpc::controller
{

    const std::unordered_map<MidiFootswitchFunction, hardware::ComponentId>
        footswitchToComponentId{
            {MidiFootswitchFunction::PLAY_START,
             hardware::ComponentId::PLAY_START},
            {MidiFootswitchFunction::PLAY, hardware::ComponentId::PLAY},
            {MidiFootswitchFunction::STOP, hardware::ComponentId::STOP},
            {MidiFootswitchFunction::TAP,
             hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT},

            {MidiFootswitchFunction::PAD_BANK_A, hardware::ComponentId::BANK_A},
            {MidiFootswitchFunction::PAD_BANK_B, hardware::ComponentId::BANK_B},
            {MidiFootswitchFunction::PAD_BANK_C, hardware::ComponentId::BANK_C},
            {MidiFootswitchFunction::PAD_BANK_D, hardware::ComponentId::BANK_D},

            {MidiFootswitchFunction::F1, hardware::ComponentId::F1},
            {MidiFootswitchFunction::F2, hardware::ComponentId::F2},
            {MidiFootswitchFunction::F3, hardware::ComponentId::F3},
            {MidiFootswitchFunction::F4, hardware::ComponentId::F4},
            {MidiFootswitchFunction::F5, hardware::ComponentId::F5},
            {MidiFootswitchFunction::F6, hardware::ComponentId::F6},

            // Pads 1–16
            {MidiFootswitchFunction::PAD_1, hardware::ComponentId::PAD_1_OR_AB},
            {MidiFootswitchFunction::PAD_2,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 1)},
            {MidiFootswitchFunction::PAD_3,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 2)},
            {MidiFootswitchFunction::PAD_4,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 3)},
            {MidiFootswitchFunction::PAD_5,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 4)},
            {MidiFootswitchFunction::PAD_6,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 5)},
            {MidiFootswitchFunction::PAD_7,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 6)},
            {MidiFootswitchFunction::PAD_8,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 7)},
            {MidiFootswitchFunction::PAD_9,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 8)},
            {MidiFootswitchFunction::PAD_10,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 9)},
            {MidiFootswitchFunction::PAD_11,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 10)},
            {MidiFootswitchFunction::PAD_12,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 11)},
            {MidiFootswitchFunction::PAD_13,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 12)},
            {MidiFootswitchFunction::PAD_14,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 13)},
            {MidiFootswitchFunction::PAD_15,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 14)},
            {MidiFootswitchFunction::PAD_16,
             static_cast<hardware::ComponentId>(
                 static_cast<int>(hardware::ComponentId::PAD_1_OR_AB) + 15)},
        };

    const std::unordered_map<
        MidiFootswitchFunction,
        input::midi::MidiControlTarget::SequencerTarget::Command>
        footswitchToSequencerCmd{
            {MidiFootswitchFunction::REC_PLAY,
             input::midi::MidiControlTarget::SequencerTarget::Command::
                 REC_PLUS_PLAY},
            {MidiFootswitchFunction::ODUB_PLAY,
             input::midi::MidiControlTarget::SequencerTarget::Command::
                 ODUB_PLUS_PLAY},
            {MidiFootswitchFunction::REC_PUNCH,
             input::midi::MidiControlTarget::SequencerTarget::Command::
                 REC_PUNCH},
            {MidiFootswitchFunction::ODUB_PUNCH,
             input::midi::MidiControlTarget::SequencerTarget::Command::
                 ODUB_PUNCH},
        };

    const std::unordered_map<MidiFootswitchFunction, std::string>
        functionNameFromEnum{
            {MidiFootswitchFunction::PLAY_START, "PLAY STRT"},
            {MidiFootswitchFunction::PLAY, "PLAY"},
            {MidiFootswitchFunction::STOP, "STOP"},
            {MidiFootswitchFunction::REC_PLAY, "REC+PLAY"},
            {MidiFootswitchFunction::ODUB_PLAY, "ODUB+PLAY"},
            {MidiFootswitchFunction::REC_PUNCH, "REC/PUNCH"},
            {MidiFootswitchFunction::ODUB_PUNCH, "ODUB/PNCH"},
            {MidiFootswitchFunction::TAP, "TAP"},
            {MidiFootswitchFunction::PAD_BANK_A, "PAD BNK A"},
            {MidiFootswitchFunction::PAD_BANK_B, "PAD BNK B"},
            {MidiFootswitchFunction::PAD_BANK_C, "PAD BNK C"},
            {MidiFootswitchFunction::PAD_BANK_D, "PAD BNK D"},
            {MidiFootswitchFunction::PAD_1, " PAD   1"},
            {MidiFootswitchFunction::PAD_2, " PAD   2"},
            {MidiFootswitchFunction::PAD_3, " PAD   3"},
            {MidiFootswitchFunction::PAD_4, " PAD   4"},
            {MidiFootswitchFunction::PAD_5, " PAD   5"},
            {MidiFootswitchFunction::PAD_6, " PAD   6"},
            {MidiFootswitchFunction::PAD_7, " PAD   7"},
            {MidiFootswitchFunction::PAD_8, " PAD   8"},
            {MidiFootswitchFunction::PAD_9, " PAD   9"},
            {MidiFootswitchFunction::PAD_10, " PAD  10"},
            {MidiFootswitchFunction::PAD_11, " PAD  11"},
            {MidiFootswitchFunction::PAD_12, " PAD  12"},
            {MidiFootswitchFunction::PAD_13, " PAD  13"},
            {MidiFootswitchFunction::PAD_14, " PAD  14"},
            {MidiFootswitchFunction::PAD_15, " PAD  15"},
            {MidiFootswitchFunction::PAD_16, " PAD  16"},
            {MidiFootswitchFunction::F1, "   F1"},
            {MidiFootswitchFunction::F2, "   F2"},
            {MidiFootswitchFunction::F3, "   F3"},
            {MidiFootswitchFunction::F4, "   F4"},
            {MidiFootswitchFunction::F5, "   F5"},
            {MidiFootswitchFunction::F6, "   F6"},
        };

} // namespace mpc::controller
