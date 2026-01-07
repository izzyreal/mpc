#include "controller/ClientExtendedMidiController.hpp"

#include "controller/ClientHardwareEventController.hpp"

#include "sequencer/Sequencer.hpp"

#include "input/midi/MidiControlPresetV3.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"

using namespace mpc::controller;
using namespace mpc::input::midi;
using namespace mpc::client::event;
using namespace mpc::hardware;

ClientExtendedMidiController::ClientExtendedMidiController(
    const std::shared_ptr<ClientHardwareEventController>
        &clientHardwareEventController,
    const std::weak_ptr<sequencer::Sequencer> &sequencer)
    : clientHardwareEventController(clientHardwareEventController),
      sequencer(sequencer)
{
    activePreset = std::make_shared<MidiControlPresetV3>();
    MidiControlPresetUtil::resetMidiControlPreset(activePreset);
    activePreset->setName("active_preset");
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

void ClientExtendedMidiController::handleEvent(const ClientMidiEvent &e)
{
    if (e.getMessageType() != ClientMidiEvent::NOTE_ON &&
        e.getMessageType() != ClientMidiEvent::NOTE_OFF &&
        e.getMessageType() != ClientMidiEvent::CONTROLLER)
    {
        return;
    }

    const auto &bindings = activePreset->getBindings();

    using Id = hardware::ComponentId;

    for (auto &b : bindings)
    {
        if (b.getMidiNumber() == NoMidiNumber)
        {
            continue;
        }

        bool hasStatefulDataWheelBindingForController = false;

        if (b.getMessageType() == "CC")
        {
            if (e.getMessageType() != ClientMidiEvent::CONTROLLER ||
                b.getMidiNumber() != e.getControllerNumber())
            {
                continue;
            }

            hasStatefulDataWheelBindingForController =
                activePreset->hasStatefulDataWheelBindingForController(
                    MidiNumber(e.getControllerNumber()));
        }
        else if (b.getMessageType() == "Note")
        {
            if ((e.getMessageType() != ClientMidiEvent::NOTE_ON &&
                 e.getMessageType() != ClientMidiEvent::NOTE_OFF) ||
                b.getMidiNumber() != e.getNoteNumber())
            {
                continue;
            }
        }

        if (auto hardwareTarget = b.getHardwareTarget())
        {
            const auto componentId =
                hardware::componentLabelToId.at(*hardwareTarget);

            if (componentId == Id::NONE)
            {
                continue;
            }

            static const std::vector<Id> potComponents{SLIDER, MAIN_VOLUME_POT,
                                                       REC_GAIN_POT};

            if (componentId == DATA_WHEEL)
            {
                if (hasStatefulDataWheelBindingForController)
                {
                    if (statefulDataWheels.count(
                            MidiNumber(e.getControllerNumber())) == 0)
                    {
                        statefulDataWheels[MidiNumber(
                            e.getControllerNumber())] =
                            MidiValue(e.getControllerValue());
                    }
                    else
                    {
                        const auto previousValue = statefulDataWheels.at(
                            MidiNumber(e.getControllerNumber()));

                        if (previousValue < e.getControllerValue())
                        {
                            turnWheel(1);
                        }
                        else if (previousValue > e.getControllerValue())
                        {
                            turnWheel(-1);
                        }
                        else if (previousValue == MinMidiValue)
                        {
                            turnWheel(-1);
                        }
                        else if (previousValue == MaxMidiValue)
                        {
                            turnWheel(1);
                        }

                        statefulDataWheels[MidiNumber(
                            e.getControllerNumber())] =
                            MidiValue(e.getControllerValue());
                    }
                }
                else
                {
                    const auto direction = b.getHardwareDirection();

                    if (direction == input::Direction::NoDirection)
                    {
                    }
                    else if (direction == input::Direction::Positive)
                    {
                        turnWheel(1);
                    }
                    else
                    {
                        turnWheel(-1);
                    }
                }
            }
            else if (std::find(potComponents.begin(), potComponents.end(),
                               componentId) != potComponents.end())
            {
                if (componentId == SLIDER)
                {
                    moveSlider(1.f - (e.getControllerValue() / 127.f));
                }
            }
            else
            {
                bool isButton = false;
                bool isPad = false;

                if (componentId >= Id::PAD_1_OR_AB &&
                    componentId <= PAD_16_OR_PARENTHESES)
                {
                    isPad = true;
                }
                else if (componentId >= Id::CURSOR_LEFT_OR_DIGIT &&
                         componentId <= NUM_9_OR_MIDI_SYNC)
                {
                    isButton = true;
                }

                if (e.getMessageType() == ClientMidiEvent::NOTE_ON)
                {
                    if (isButton)
                    {
                        pressButton(componentId);
                    }
                    else if (isPad)
                    {
                        pressPad(componentId, e.getVelocity());
                    }
                }
                else if (e.getMessageType() == ClientMidiEvent::NOTE_OFF)
                {
                    if (isButton)
                    {
                        releaseButton(componentId);
                    }
                    else if (isPad)
                    {
                        releasePad(componentId);
                    }
                }
                else
                {
                    if (e.getControllerValue() >= 64)
                    {
                        if (isButton)
                        {
                            pressButton(componentId);
                        }
                        else if (isPad)
                        {
                            pressPad(componentId, 1.f);
                        }
                    }
                    else
                    {
                        if (isButton)
                        {
                            releaseButton(componentId);
                        }
                        else if (isPad)
                        {
                            releasePad(componentId);
                        }
                    }
                }
            }
        }
        else if (auto sequencerTarget = b.getSequencerTarget())
        {
        }
    }
}

void ClientExtendedMidiController::pressButton(ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::MpcButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::releaseButton(ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::MpcButtonRelease;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::turnWheel(int i) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = DATA_WHEEL;
    ev.type = ClientHardwareEvent::Type::DataWheelTurn;
    ev.deltaValue = i;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::moveSlider(float normalizedY) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = SLIDER;
    ev.type = ClientHardwareEvent::Type::SliderMove;
    ev.value = normalizedY;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::pressPad(hardware::ComponentId id,
                                            float normalizedVelocity) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::PadPress;
    ev.index = static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
    ev.value = normalizedVelocity;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::releasePad(hardware::ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::PadRelease;
    ev.index = static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
    clientHardwareEventController->handleClientHardwareEvent(ev);
}
