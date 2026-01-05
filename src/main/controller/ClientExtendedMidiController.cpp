#include "controller/ClientExtendedMidiController.hpp"

#include "input/midi/MidiControlPresetV3.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"

using namespace mpc::controller;
using namespace mpc::input::midi;

ClientExtendedMidiController::ClientExtendedMidiController()
{
    activePreset = std::make_shared<MidiControlPresetV3>();
    MidiControlPresetUtil::resetMidiControlPreset(activePreset);
}

std::shared_ptr<MidiControlPresetV3>
ClientExtendedMidiController::getActivePreset()
{
    return activePreset;
}

void ClientExtendedMidiController::setActivePreset(
    std::shared_ptr<MidiControlPresetV3> p)
{
    activePreset = p;
}
