#pragma once

#include <cstdint>

namespace mpc::client::event
{
    struct ClientMidiEvent
    {
        enum MessageType
        {
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
        ClientMidiEvent() = default;
        explicit ClientMidiEvent(const MessageType type, const int d1 = 0,
                                 const int d2 = 0)
            : messageType(type), data1(static_cast<uint8_t>(d1)),
              data2(static_cast<uint8_t>(d2))
        {
        }

        MessageType getMessageType() const noexcept
        {
            return messageType;
        }
        void setMessageType(const MessageType type) noexcept
        {
            messageType = type;
        }

        void setChannel(const int c) noexcept
        {
            channel = static_cast<uint8_t>(c);
        }
        int getChannel() const noexcept
        {
            return channel;
        }

        void setBufferOffset(const double b) noexcept
        {
            bufferOffset = b;
        }

        double getBufferOffset() const noexcept
        {
            return bufferOffset;
        }

        int getNoteNumber() const noexcept
        {
            return data1;
        }

        void setNoteNumber(const int n) noexcept
        {
            data1 = static_cast<uint8_t>(n);
        }

        int getVelocity() const noexcept
        {
            return data2;
        }

        void setVelocity(const int v) noexcept
        {
            data2 = static_cast<uint8_t>(v);
        }

        int getControllerNumber() const noexcept
        {
            return data1;
        }

        void setControllerNumber(const int c) noexcept
        {
            data1 = static_cast<uint8_t>(c);
        }

        int getControllerValue() const noexcept
        {
            return data2;
        }

        void setControllerValue(const int v) noexcept
        {
            data2 = static_cast<uint8_t>(v);
        }

        int getAftertouchNote() const noexcept
        {
            return data1;
        }

        void setAftertouchNote(const int n) noexcept
        {
            data1 = static_cast<uint8_t>(n);
        }

        int getAftertouchValue() const noexcept
        {
            return data2;
        }

        void setAftertouchValue(const int v) noexcept
        {
            data2 = static_cast<uint8_t>(v);
        }

        int getChannelPressure() const noexcept
        {
            return data1;
        }

        void setChannelPressure(const int p) noexcept
        {
            data1 = static_cast<uint8_t>(p);
        }

        int getProgramNumber() const noexcept
        {
            return data1;
        }

        void setProgramNumber(const int n) noexcept
        {
            data1 = static_cast<uint8_t>(n);
        }

        int getPitchWheelValue() const noexcept
        {
            return (data2 << 7 | data1) - 8192;
        }

        void setPitchWheelValue(int v) noexcept
        {
            v += 8192;
            data1 = static_cast<uint8_t>(v & 0x7F);
            data2 = static_cast<uint8_t>(v >> 7 & 0x7F);
        }

        void printInfo() const noexcept;
    };
} // namespace mpc::client::event
