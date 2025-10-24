#include "input/MidiEvent.hpp"

#include "Logger.hpp"

#include <cstdio>
#include <string>

using namespace mpc::input;

void MidiEvent::printInfo() const noexcept
{
    static const char *typeNames[] = {
        "NOTE_ON", "NOTE_OFF", "CONTROLLER", "AFTERTOUCH",
        "CHANNEL_PRESSURE", "MIDI_CLOCK", "MIDI_START",
        "MIDI_STOP", "MIDI_CONTINUE", "PROGRAM_CHANGE", "PITCH_WHEEL"};

    std::string msg = "MIDI Event: " + std::string(typeNames[messageType]) +
                      " | Ch: " + std::to_string(channel) +
                      " | Offset: " + std::to_string(bufferOffset) + " | ";

    switch (messageType)
    {
    case NOTE_ON:
    case NOTE_OFF:
        msg += "Note: " + std::to_string(getNoteNumber()) +
               " | Vel: " + std::to_string(getVelocity());
        break;
    case CONTROLLER:
        msg += "CC#: " + std::to_string(getControllerNumber()) +
               " | Value: " + std::to_string(getControllerValue());
        break;
    case AFTERTOUCH:
        msg += "Note: " + std::to_string(getAftertouchNote()) +
               " | Pressure: " + std::to_string(getAftertouchValue());
        break;
    case CHANNEL_PRESSURE:
        msg += "Pressure: " + std::to_string(getChannelPressure());
        break;
    case PROGRAM_CHANGE:
        msg += "Program: " + std::to_string(getProgramNumber());
        break;
    case PITCH_WHEEL:
        msg += "Pitch: " + std::to_string(getPitchWheelValue());
        break;
    default:
        msg += "No data bytes";
        break;
    }

    std::printf("%s\n", msg.c_str());
    MLOG(msg);
}
