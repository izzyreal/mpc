#include "MidiControlCommand.hpp"

using namespace mpc::nvram;

bool MidiControlCommand::isEmpty() {
    return !isNote && channel == -2 && value == -2;
}

void MidiControlCommand::reset() {
    isNote = false;
    channel = -2;
    value = -2;
}

MidiControlCommand::MidiControlCommand(std::string newLabel, bool newIsNote, int newChannelIndex, int newValue) : label(std::move(newLabel)),
isNote(newIsNote),
channel(newChannelIndex),
value(newValue) {}

MidiControlCommand::MidiControlCommand(const MidiControlCommand &c) : label(c.label), isNote(c.isNote), channel(c.channel), value(c.value) {}

bool MidiControlCommand::equals(const MidiControlCommand &other) {
    return isNote == other.isNote && channel == other.channel && value == other.value;
}

std::vector<char> MidiControlCommand::toBytes() {
    std::vector<char> result;

    for (int i = 0; i < label.size(); i++) {
        result.push_back(label[i]);
    }

    result.push_back(' ');

    result.push_back(isNote ? 1 : 0);
    result.push_back(channel);
    result.push_back(value);
    return result;
}