#pragma once

#include <cstdint>
#include <cstdio>

namespace mpc::input {
    struct MidiEvent {
        enum MessageType {
            NOTE_ON,
            NOTE_OFF,
            CONTROLLER,
            AFTERTOUCH,
            CHANNEL_PRESSURE,
            MIDI_CLOCK,
            MIDI_START,
            MIDI_STOP,
            MIDI_CONTINUE,
            PROGRAM_CHANGE,
            PITCH_WHEEL
        };

        private:
        MessageType messageType{};
        uint8_t channel{};
        uint8_t data1{};
        uint8_t data2{};
        double bufferOffset{};

        public:
        MidiEvent() = default;
        MidiEvent(MessageType type, int d1 = 0, int d2 = 0)
            : messageType(type), data1(static_cast<uint8_t>(d1)), data2(static_cast<uint8_t>(d2)) {}

        MessageType getMessageType() const noexcept { return messageType; }
        void setMessageType(MessageType type) noexcept { messageType = type; }
        // 0 - 15
        void setChannel(const int c) noexcept { channel = c; }
        void setBufferOffset(const double b) noexcept { bufferOffset = b; }

        // --- Logical getters/setters depending on message type ---
        int getNoteNumber() const noexcept { return static_cast<int>(data1); }
        void setNoteNumber(int n) noexcept { data1 = static_cast<uint8_t>(n); }

        int getVelocity() const noexcept { return static_cast<int>(data2); }
        void setVelocity(int v) noexcept { data2 = static_cast<uint8_t>(v); }

        int getControllerNumber() const noexcept { return static_cast<int>(data1); }
        void setControllerNumber(int c) noexcept { data1 = static_cast<uint8_t>(c); }

        int getControllerValue() const noexcept { return static_cast<int>(data2); }
        void setControllerValue(int v) noexcept { data2 = static_cast<uint8_t>(v); }

        int getAftertouchNote() const noexcept { return static_cast<int>(data1); }
        void setAftertouchNote(int n) noexcept { data1 = static_cast<uint8_t>(n); }

        int getAftertouchValue() const noexcept { return static_cast<int>(data2); }
        void setAftertouchValue(int v) noexcept { data2 = static_cast<uint8_t>(v); }

        int getChannelPressure() const noexcept { return static_cast<int>(data1); }
        void setChannelPressure(int p) noexcept { data1 = static_cast<uint8_t>(p); }

        int getProgramNumber() const noexcept { return static_cast<int>(data1); }
        void setProgramNumber(int n) noexcept { data1 = static_cast<uint8_t>(n); }

        int getPitchWheelValue() const noexcept
        {
            return static_cast<int>((data2 << 7) | data1) - 8192;
        }
        
        void setPitchWheelValue(int v) noexcept
        {
            v += 8192;
            data1 = static_cast<uint8_t>(v & 0x7F);
            data2 = static_cast<uint8_t>((v >> 7) & 0x7F);
        }

        void printInfo() const noexcept
        {
            static const char* typeNames[] = {
                "NOTE_ON", "NOTE_OFF", "CONTROLLER", "AFTERTOUCH",
                "CHANNEL_PRESSURE", "MIDI_CLOCK", "MIDI_START",
                "MIDI_STOP", "MIDI_CONTINUE", "PROGRAM_CHANGE", "PITCH_WHEEL"
            };

            std::printf("MIDI Event: %s | Ch: %d | Offset: %.3f | ",
                        typeNames[messageType], channel, bufferOffset);

            switch (messageType)
            {
                case NOTE_ON:
                case NOTE_OFF:
                    std::printf("Note: %d | Vel: %d\n", getNoteNumber(), getVelocity());
                    break;
                case CONTROLLER:
                    std::printf("CC#: %d | Value: %d\n", getControllerNumber(), getControllerValue());
                    break;
                case AFTERTOUCH:
                    std::printf("Note: %d | Pressure: %d\n", getAftertouchNote(), getAftertouchValue());
                    break;
                case CHANNEL_PRESSURE:
                    std::printf("Pressure: %d\n", getChannelPressure());
                    break;
                case PROGRAM_CHANGE:
                    std::printf("Program: %d\n", getProgramNumber());
                    break;
                case PITCH_WHEEL:
                    std::printf("Pitch: %d\n", getPitchWheelValue());
                    break;
                default:
                    std::printf("No data bytes\n");
                    break;
            }
        }
    };
} // namespace mpc::input
