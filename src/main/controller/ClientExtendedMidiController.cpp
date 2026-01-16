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
    const std::shared_ptr<MidiControlPresetV3> &p)
{
    activePreset = p;
}

void ClientExtendedMidiController::handleEvent(const ClientMidiEvent &e)
{
    if (e.getMessageType() != ClientMidiEvent::NOTE_ON &&
        e.getMessageType() != ClientMidiEvent::NOTE_OFF &&
        e.getMessageType() != ClientMidiEvent::AFTERTOUCH &&
        e.getMessageType() != ClientMidiEvent::CONTROLLER)
    {
        return;
    }

    const auto &bindings = activePreset->getBindings();

    for (auto &b : bindings)
    {
        if (!b.isEnabled())
        {
            continue;
        }

        if (b.isController())
        {
            if (e.getMessageType() != ClientMidiEvent::CONTROLLER ||
                b.getMidiNumber() != e.getControllerNumber())
            {
                continue;
            }
        }
        else if (b.isNote())
        {
            if ((e.getMessageType() != ClientMidiEvent::NOTE_ON &&
                 e.getMessageType() != ClientMidiEvent::NOTE_OFF &&
                 e.getMessageType() != ClientMidiEvent::AFTERTOUCH) ||
                b.getMidiNumber() != e.getNoteNumber())
            {
                continue;
            }
        }

        if (auto hardwareTarget = b.getHardwareTarget())
        {
            const auto componentId = componentLabelToId.at(*hardwareTarget);

            if (componentId == NONE)
            {
                continue;
            }

            static const std::vector potComponents{SLIDER, MAIN_VOLUME_POT,
                                                   REC_GAIN_POT};

            if (componentId == DATA_WHEEL)
            {
                if (const auto direction = b.getHardwareDirection();
                    direction == input::Direction::Positive)
                {
                    turnWheel(1);
                }
                else if (direction == input::Direction::Negative)
                {
                    turnWheel(-1);
                }
                else /* if (direction == input::Direction::None) */
                {
                    if (b.encoderMode == BindingEncoderMode::RelativeStateful)
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
                    else /*if (b.getEncoderMode() ==
                            BindingEncoderMode::RelativeStateless)*/
                    {
                        auto increment = e.getControllerValue();

                        if (increment > 63)
                        {
                            increment = -(128 - increment);
                        }

                        turnWheel(increment);
                    }
                }
            }
            else if (std::find(potComponents.begin(), potComponents.end(),
                               componentId) != potComponents.end())
            {
                if (componentId == SLIDER)
                {
                    moveSlider(1.f - e.getControllerValue() / 127.f);
                }
                else if (componentId == REC_GAIN_POT ||
                         componentId == MAIN_VOLUME_POT)
                {
                    movePot(componentId, e.getControllerValue() / 127.f);
                }
            }
            else
            {
                bool isButton = false;
                bool isPad = false;

                if (isPadId(componentId))
                {
                    isPad = true;
                }
                else if (componentId >= CURSOR_LEFT_OR_DIGIT &&
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
                        constexpr bool isAftertouch = false;
                        pressPad(componentId, e.getVelocity() / 127.f,
                                 isAftertouch);
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
                else if (e.getMessageType() == ClientMidiEvent::AFTERTOUCH)
                {
                    constexpr bool isAftertouch = true;
                    pressPad(componentId, e.getAftertouchValue() / 127.f,
                             isAftertouch);
                }
                else
                {
                    if (e.getControllerValue() >= b.getMidiValue())
                    {
                        if (isButton)
                        {
                            pressButton(componentId);
                        }
                        else if (isPad)
                        {
                            constexpr bool isAftertouch = false;
                            pressPad(componentId, 1.f, isAftertouch);
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
    }
}

void ClientExtendedMidiController::pressButton(const ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::MpcButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::releaseButton(const ComponentId id) const
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

void ClientExtendedMidiController::pressPad(const ComponentId id,
                                            const float normalizedVelocity,
                                            const bool isAftertouch) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = isAftertouch ? ClientHardwareEvent::Type::PadAftertouch
                           : ClientHardwareEvent::Type::PadPress;
    ev.index = static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
    ev.value = normalizedVelocity;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::releasePad(const ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::PadRelease;
    ev.index = static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientExtendedMidiController::movePot(const ComponentId id,
                                           float normalizedValue) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::PotMove;
    ev.value = normalizedValue;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}
