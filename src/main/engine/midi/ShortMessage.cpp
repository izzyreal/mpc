#include "ShortMessage.hpp"

#include <string>

using namespace std;
using namespace mpc::engine::midi;

ShortMessage::ShortMessage()
    : ShortMessage(vector<char>(3))
{
    data[0] = (NOTE_ON & 0xFF);
    data[1] = 64;
    data[2] = 127;
    length = 3;
}

ShortMessage::ShortMessage(const vector<char> &data)
    : MidiMessage(data)
{
}

int ShortMessage::getChannel()
{
    return (getStatus() & 0x0F);
}

int ShortMessage::getCommand()
{
    return (getStatus() & 0xF0);
}

int ShortMessage::getData1()
{
    if (length > 1)
    {
        return (data[1] & 0xFF);
    }
    return 0;
}

int ShortMessage::getData2()
{
    if (length > 2)
    {
        return (data[2] & 0xFF);
    }
    return 0;
}

bool mpc::engine::midi::ShortMessage::isNoteOn()
{
    return getCommand() == NOTE_ON;
}

bool mpc::engine::midi::ShortMessage::isNoteOff()
{
    return getCommand() == NOTE_OFF;
}

bool mpc::engine::midi::ShortMessage::isMidiClock()
{
    const auto s = getStatus();
    return s == ShortMessage::TIMING_CLOCK || s == ShortMessage::START || s == ShortMessage::STOP || s == ShortMessage::CONTINUE;
}

bool mpc::engine::midi::ShortMessage::isControlChange()
{
    const auto s = getStatus();
    return s >= ShortMessage::CONTROL_CHANGE && s < ShortMessage::CONTROL_CHANGE + 16;
}

bool mpc::engine::midi::ShortMessage::isChannelPressure()
{
    const auto s = getStatus();
    return s >= ShortMessage::CHANNEL_PRESSURE && s < ShortMessage::CHANNEL_PRESSURE + 16;
}

int ShortMessage::getDataLength(int status)
{
    // system common and system real-time messages
    switch (status)
    {
    case 0xF6: // Tune Request
    case 0xF7: // EOX
               // System real-time messages
    case 0xF8: // Timing Clock
    case 0xF9: // Undefined
    case 0xFA: // Start
    case 0xFB: // Continue
    case 0xFC: // Stop
    case 0xFD: // Undefined
    case 0xFE: // Active Sensing
    case 0xFF: // System Reset
        return 0;
    case 0xF1: // MTC Quarter Frame
    case 0xF3: // Song Select
        return 1;
    case 0xF2: // Song Position Pointer
        return 2;
    default:
        break;
    }

    // channel voice and mode messages
    switch (status & 0xF0)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0:
        return 2;
    case 0xC0:
    case 0xD0:
        return 1;
    default:
        return -1;
    }
}

void ShortMessage::setMessage(int status)
{
    int dataLength = getDataLength(status);
    if (dataLength != 0)
    {
        string error = "Status byte; " + to_string(status) + " requires " + to_string(dataLength) + " data bytes";
        return;
    }
    setMessage(status, 0, 0);
}

void ShortMessage::setMessage(int status, int data1, int data2)
{
    int dataLength = getDataLength(status); // can throw InvalidMidiDataException
    if (dataLength > 0)
    {
        if (data1 < 0 || data1 > 127)
        {
            string error = "data1 out of range: " + to_string(data1);
            return;
        }
        if (dataLength > 1)
        {
            if (data2 < 0 || data2 > 127)
            {
                string error = "data2 out of range: " + to_string(data2);
                return;
            }
        }
    }

    length = dataLength + 1;

    if (data.size() < length)
    {
        data = vector<char>(3);
    }

    data[0] = (status & 0xFF);

    if (length > 1)
    {
        data[1] = (data1 & 0xFF);

        if (length > 2)
        {
            data[2] = (data2 & 0xFF);
        }
    }
}

void ShortMessage::setMessage(int command, int channel, int data1, int data2)
{
    if (command >= 0xF0 || command < 0x80)
    {
        string error = "command out of range: " + to_string(command);
        return;
    }

    if ((channel & 0xFFFFFFF0) != 0)
    { // <=> (channel<0 || channel>15)
        string error = "channel out of range: " + to_string(channel);
        return;
    }

    setMessage((command & 0xF0) | (channel & 0x0F), data1, data2);
}

void ShortMessage::setMessage(const vector<char> &data, int length)
{
    this->data = data;
    this->length = length;
}
