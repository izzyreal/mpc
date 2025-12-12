#pragma once

#include <cstdint>
#include <string>

namespace mpc
{
    struct Mpc2000XlSpecs
    {
        static constexpr uint16_t MAX_SOUND_COUNT_IN_MEMORY = 256;
        static constexpr uint16_t MAX_PROGRAM_COUNT = 24;
        static constexpr uint16_t MAX_LAST_PROGRAM_INDEX =
            MAX_PROGRAM_COUNT - 1;
        static constexpr uint16_t PROGRAM_BANK_COUNT = 4;
        static constexpr uint16_t HARDWARE_PAD_COUNT = 16;
        static constexpr int8_t PADS_PER_BANK_COUNT = 16;
        static constexpr uint16_t PROGRAM_PAD_COUNT =
            PROGRAM_BANK_COUNT * PADS_PER_BANK_COUNT;
        static constexpr uint8_t FIRST_DRUM_NOTE = 35;
        static constexpr uint8_t LAST_DRUM_NOTE = 98;
        static constexpr uint8_t MUTE_ASSIGN_DISABLED = 34;
        static constexpr uint8_t OPTIONAL_NOTE_DISABLED = 34;
        static constexpr uint8_t MIDI_BUS_COUNT = 1;
        static constexpr uint8_t DRUM_BUS_COUNT = 4;
        static constexpr uint8_t TOTAL_BUS_COUNT =
            MIDI_BUS_COUNT + DRUM_BUS_COUNT;
        static constexpr uint8_t LAST_BUS_INDEX = TOTAL_BUS_COUNT - 1;
        static constexpr uint8_t TRACK_COUNT = 64;
        static constexpr int8_t LAST_TRACK_INDEX = TRACK_COUNT - 1;
        static constexpr int8_t SEQUENCE_COUNT = 99;
        static constexpr uint16_t MAX_BAR_COUNT = 999;
        static constexpr uint16_t MAX_LAST_BAR_INDEX = MAX_BAR_COUNT - 1;
        static constexpr int8_t LAST_SEQUENCE_INDEX = SEQUENCE_COUNT - 1;
        static constexpr uint16_t MAX_NOTE_EVENT_DURATION = 9999;
        static constexpr int64_t GLOBAL_EVENT_CAPACITY = 262'144;
        static constexpr int16_t SEQUENCER_RESOLUTION_PPQ = 96;

        // We use just one meta track in our sequences: tempo change.
        static constexpr int8_t META_TRACK_COUNT = 1; //

        static constexpr int8_t TOTAL_TRACK_COUNT =
            TRACK_COUNT + META_TRACK_COUNT;

        static constexpr int8_t META_SEQUENCE_COUNT =
            2; // Undo sequence placeholder and copy staging and load staging

        static constexpr int8_t TOTAL_SEQUENCE_COUNT =
            SEQUENCE_COUNT + META_SEQUENCE_COUNT;

        static constexpr int8_t MAX_SONG_COUNT = 20;
        static constexpr uint8_t MAX_SONG_STEP_COUNT = 250;
        static constexpr int8_t MAX_SONG_STEP_REPETITION_COUNT = 99;

        static constexpr int8_t MAX_TRACK_NAME_LENGTH = 16;
        static constexpr int8_t MAX_SEQUENCE_NAME_LENGTH = 16;
        static constexpr int8_t MAX_SONG_NAME_LENGTH = 16;
        static constexpr int8_t MAX_PROGRAM_NAME_LENGTH = 16;

        static constexpr int8_t MAX_SOUND_NAME_LENGTH = 16;

        static constexpr double MIN_TEMPO = 30.0;
        static constexpr double MAX_TEMPO = 300.0;
        static constexpr double DEFAULT_TEMPO = 120.0;
    };
} // namespace mpc
