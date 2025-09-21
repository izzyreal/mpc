#include "MidiControlCommand.hpp"
#include <stdexcept>

using namespace mpc::nvram;

MidiControlCommand::MidiControlCommand(const std::string mpcHardwareLabelToUse,
                                       const MidiMessageType midiMessageTypeToUse,
                                       const int8_t midiChannelIndexToUse,
                                       const int8_t numberToUse) :
    mpcHardwareLabel(std::move(mpcHardwareLabelToUse)),
    midiMessageType(midiMessageTypeToUse),
    midiChannelIndex(midiChannelIndexToUse),
    number(numberToUse),
    value(-2)
{
    if (midiMessageType != NOTE)
    {
        throw std::invalid_argument("A MidiControlCommand without a value must be of type NOTE");
    }
}

MidiControlCommand::MidiControlCommand(const std::string mpcHardwareLabelToUse,
                                       const MidiMessageType midiMessageTypeToUse,
                                       const int8_t midiChannelIndexToUse,
                                       const int8_t numberToUse,
                                       const int8_t valueToUse) :
    mpcHardwareLabel(std::move(mpcHardwareLabelToUse)),
    midiMessageType(midiMessageTypeToUse),
    midiChannelIndex(midiChannelIndexToUse),
    number(numberToUse),
    value(valueToUse)
{
    if (midiMessageType != CC)
    {
        throw std::invalid_argument("A MidiControlCommand with a value must be of type CC");
    }
}

MidiControlCommand::MidiControlCommand(const MidiControlCommand &c) :
    mpcHardwareLabel(c.mpcHardwareLabel),
    midiMessageType(c.midiMessageType),
    midiChannelIndex(c.midiChannelIndex),
    number(c.number),
    value(c.value)
{
    if (midiMessageType == NOTE && value != -2)
    {
        throw std::invalid_argument("A MidiControlCommand of type NOTE may not have a value");
    }
    
    if (midiMessageType == CC && value == -2)
    {
        throw std::invalid_argument("A MidiControlCommand of type CC must have a value");
    }
}

bool MidiControlCommand::equals(const MidiControlCommand &other) const
{
    return midiMessageType == other.midiMessageType &&
           midiChannelIndex == other.midiChannelIndex &&
           number == other.number &&
           value == other.value;
}

bool MidiControlCommand::isEmpty() const
{
    return midiMessageType == NONE && midiChannelIndex == -2 && number == -2 && value == -2;
}

void MidiControlCommand::reset()
{
    midiMessageType = NONE;
    midiChannelIndex = -2;
    number = -2;
    value = -2;
}

std::vector<char> MidiControlCommand::toBytes() const
{
    if (midiMessageType == NONE)
    {
        throw std::runtime_error("A MidiControlCommand of type NONE should not be serialized");
    }

    if (midiMessageType == NOTE && value != -2)
    {
        throw std::invalid_argument("A MidiControlCommand of type NOTE may not have a value");
    }
    
    if (midiMessageType == CC && value == -2)
    {
        throw std::invalid_argument("A MidiControlCommand of type CC must have a value");
    }

    std::vector<char> result;

    for (int i = 0; i < mpcHardwareLabel.size(); i++)
    {
        result.push_back(mpcHardwareLabel[i]);
    }

    result.push_back(' ');

    result.push_back(midiMessageType == NOTE ? 1 : 0);
    result.push_back(midiChannelIndex);
    result.push_back(number);
    result.push_back(value);
    return result;
}

MidiControlCommand MidiControlCommand::fromBytes(const std::vector<char> &bytes)
{
    std::string mpcHardwareLabel;

    char c;

    size_t pointer = 0;

    while ((c = bytes[pointer++]) != ' ' && pointer < bytes.size())
    {
        mpcHardwareLabel.push_back(c);
    }

    const MidiMessageType midiMessageType = bytes[pointer++] == 1 ? NOTE : CC;
    const int8_t midiChannelIndex = bytes[pointer++];
    const int8_t number = bytes[pointer++];

    if (midiChannelIndex < -1 || midiChannelIndex > 15)
    {
        throw std::runtime_error("Erroneous MIDI channel index in persisted MIDI control preset command");
    }

    if (number < -1 || number > 127)
    {
        throw std::runtime_error("Erroneous MIDI note or CC number in persisted MIDI control preset command");
    }

    if (midiMessageType == NOTE)
    {
        return MidiControlCommand(mpcHardwareLabel,
                                  MidiControlCommand::MidiMessageType::NOTE,
                                  midiChannelIndex,
                                  number);
    }

    const int8_t value = bytes[pointer];

    return MidiControlCommand(mpcHardwareLabel,
                              CC,
                              midiChannelIndex,
                              number,
                              value);
}

std::string MidiControlCommand::getMpcHardwareLabel() const
{
    return mpcHardwareLabel;
}

MidiControlCommand::MidiMessageType MidiControlCommand::getMidiMessageType() const
{
    return midiMessageType;
}

int8_t MidiControlCommand::getMidiChannelIndex() const
{
    return midiChannelIndex;
}

int8_t MidiControlCommand::getNumber() const
{
    return number;
}

int8_t MidiControlCommand::getValue() const
{
    return value;
}

void MidiControlCommand::setMidiMessageType(const MidiMessageType midiMessageTypeToUse)
{
    if (midiMessageTypeToUse == NONE)
    {
        throw std::invalid_argument("After instantiation of a MidiControlCommand, its type may only be set to type NOTE or CC");
    }

    midiMessageType = midiMessageTypeToUse;

    if (midiMessageType == CC &&
        value == -2)
    {
        value = -1;
    }
}

void MidiControlCommand::setMidiChannelIndex(const int8_t midiChannelIndexToUse)
{
    if (midiChannelIndexToUse < -1 || midiChannelIndexToUse > 15)
    {
        throw std::invalid_argument("After instantiation of a MidiControlCommand, its midiChannelIndex may only be set to -1 to 15");
    }

    midiChannelIndex = midiChannelIndexToUse;
}

void MidiControlCommand::setNumber(const int8_t numberToUse)
{
    if (numberToUse < -1 || numberToUse > 127)
    {
        throw std::invalid_argument("After instantiation of a MidiControlCommand, its number may only be set to -1 to 127");
    }

    number = numberToUse;
}

void MidiControlCommand::setValue(const int8_t valueToUse)
{
    if (midiMessageType != CC)
    {
        throw std::invalid_argument("Only MidiControlCommands of type CC have a value");
    }

    value = valueToUse;
}

void MidiControlCommand::setMpcHardwareLabel(const std::string mpcHardwareLabelToUse)
{
    mpcHardwareLabel = mpcHardwareLabelToUse;
}

bool MidiControlCommand::isNote() const
{
    return midiMessageType == NOTE;
}

bool MidiControlCommand::isCC() const
{
    return midiMessageType == CC;
}

