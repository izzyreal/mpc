#pragma once

#include <string>
#include <vector>

namespace mpc::input::midi::legacy
{
    class MidiControlCommand
    {
    public:
        enum MidiMessageType
        {
            NONE,
            NOTE,
            CC
        };

    private:
        std::string mpcHardwareLabel;
        MidiMessageType midiMessageType = NONE;
        int8_t midiChannelIndex;

        // The MIDI note or CC number
        int8_t number;

        // The CC value.
        // This field is unused by note commands.
        int8_t value;

    public:
        static MidiControlCommand fromBytes(const std::vector<char> &bytes);

        MidiControlCommand() = default;

        MidiControlCommand(const std::string &mpcHardwareLabel, MidiMessageType,
                           int8_t midiChannelIndex, int8_t number);

        MidiControlCommand(const std::string &mpcHardwareLabel, MidiMessageType,
                           int8_t midiChannelIndex, int8_t number,
                           int8_t value);

        MidiControlCommand(const MidiControlCommand &c);

        bool equals(const MidiControlCommand &other) const;

        bool isEmpty() const;

        void reset();

        std::vector<char> toBytes() const;

        std::string getMpcHardwareLabel() const;

        MidiMessageType getMidiMessageType() const;

        bool isNote() const;

        bool isCC() const;

        int8_t getMidiChannelIndex() const;

        int8_t getNumber() const;

        int8_t getValue() const;

        void setMidiMessageType(MidiMessageType);

        void setMidiChannelIndex(int8_t midiChannelIndex);

        void setNumber(int8_t number);

        void setValue(int8_t value);

        void setMpcHardwareLabel(const std::string &mpcHardwareLabel);
    };
} // namespace mpc::input::midi::legacy
