#include "input/HostToClientTranslator.hpp"
#include "hardware/ComponentId.hpp"
#include "client/event/ClientEvent.hpp"
#include "input/KeyboardBindings.hpp"

#include "input/KeyCodeHelper.hpp"

#include "hardware/Component.hpp"

#include <stdexcept>

using namespace mpc::input;
using namespace mpc::client::event;
using namespace mpc::hardware;

HostToClientTranslator::HostToClientTranslator() {}

std::optional<ClientEvent> HostToClientTranslator::translate(
    const HostInputEvent &hostInputEvent,
    std::shared_ptr<KeyboardBindings> keyboardBindings)
{
    if (hostInputEvent.getSource() == HostInputEvent::Source::MIDI)
    {
        const auto &midi = std::get<ClientMidiEvent>(hostInputEvent.payload);
        return ClientEvent{midi};
    }

    ClientHardwareEvent clientHardwareEvent;

    switch (hostInputEvent.getSource())
    {
        case HostInputEvent::Source::KEYBOARD:
            clientHardwareEvent.source =
                ClientHardwareEvent::Source::HostInputKeyboard;
            break;
        case HostInputEvent::Source::GESTURE:
            clientHardwareEvent.source =
                ClientHardwareEvent::Source::HostInputGesture;
            break;
        case HostInputEvent::Source::FOCUS:
            clientHardwareEvent.source =
                ClientHardwareEvent::Source::HostFocusEvent;
            break;
        case HostInputEvent::Source::MIDI:
            // Handled above
        default:
            break;
    }

    switch (hostInputEvent.getSource())
    {
        case HostInputEvent::Source::FOCUS:
            clientHardwareEvent.type = ClientHardwareEvent::Type::HostFocusLost;
            break;

        case HostInputEvent::Source::GESTURE:
        {
            const auto &gesture =
                std::get<GestureEvent>(hostInputEvent.payload);

            if (gesture.componentId == NONE)
            {
                printf(
                    "A gesture without a componentId was emitted. How is that "
                    "possible?\n");
                return std::nullopt;
            }

            clientHardwareEvent.componentId = gesture.componentId;

            if (gesture.componentId >= PAD_1_OR_AB &&
                gesture.componentId <= PAD_16_OR_PARENTHESES)
            {
                clientHardwareEvent.index =
                    static_cast<int>(gesture.componentId) -
                    static_cast<int>(PAD_1_OR_AB);

                if (gesture.type == GestureEvent::Type::BEGIN ||
                    gesture.type == GestureEvent::Type::REPEAT)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::PadPress;
                    clientHardwareEvent.value = 1.f - gesture.normY;
                }
                else if (gesture.type == GestureEvent::Type::END)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::PadRelease;
                }
                else if (gesture.type == GestureEvent::Type::UPDATE)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::PadAftertouch;
                    clientHardwareEvent.value = 1.f - gesture.normY;
                }
            }
            else if (gesture.componentId == SLIDER)
            {
                if (gesture.type == GestureEvent::Type::UPDATE)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::SliderMove;

                    if (gesture.movement == GestureEvent::Movement::Relative)
                    {
                        clientHardwareEvent.deltaValue =
                            gesture.continuousDelta;
                    }
                    else /* Movement::Absolute */
                    {
                        clientHardwareEvent.value = gesture.normY;
                    }
                }
            }
            else if (gesture.componentId >= CURSOR_LEFT_OR_DIGIT &&
                     gesture.componentId <= NUM_9_OR_MIDI_SYNC)
            {
                if (gesture.type == GestureEvent::Type::REPEAT)
                {
                    if (gesture.repeatCount >= 2)
                    {
                        clientHardwareEvent.type =
                            ClientHardwareEvent::Type::ButtonDoublePress;
                    }
                }
                else if (gesture.type == GestureEvent::Type::BEGIN)
                {
                    if (gesture.repeatCount >= 2)
                    {
                        clientHardwareEvent.type =
                            ClientHardwareEvent::Type::ButtonDoublePress;
                    }
                    else
                    {
                        clientHardwareEvent.type =
                            ClientHardwareEvent::Type::MpcButtonPress;
                    }
                }
                else if (gesture.type == GestureEvent::Type::END)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::MpcButtonRelease;
                }
                else
                {
                    return std::nullopt;
                }
            }
            else if (gesture.componentId == DATA_WHEEL)
            {
                if (gesture.type == GestureEvent::Type::BEGIN)
                {
                    gestureSourceTracker.beginGesture(gesture.componentId,
                                                      gesture.sourceIndex, 1);
                }
                else if (gesture.type == GestureEvent::Type::END)
                {
                    gestureSourceTracker.endGesture(gesture.componentId,
                                                    gesture.sourceIndex);
                }
                else if (gesture.type == GestureEvent::Type::UPDATE)
                {
                    if (gestureSourceTracker.updateGesture(gesture.componentId,
                                                           gesture.sourceIndex))
                    {
                        clientHardwareEvent.type =
                            ClientHardwareEvent::Type::DataWheelTurn;

                        const int modKeyCount =
                            gesture.getModifierKeyDownCount();
                        const int gestureCount =
                            gestureSourceTracker.getGestureCount(DATA_WHEEL);
                        const bool shouldTurnFast =
                            modKeyCount > 0 || gestureCount > 1;
                        const float baseTurnFactor =
                            gesture.inputDeviceType ==
                                    GestureEvent::InputDeviceType::Mouse
                                ? .3f
                                : .1f;
                        constexpr float fastTurnFactor = 10.f;
                        const int fastTurnFactorMultiplier =
                            modKeyCount + std::max(0, gestureCount - 1);

                        const float turnSpeedFactor =
                            shouldTurnFast ? baseTurnFactor * fastTurnFactor *
                                                 fastTurnFactorMultiplier
                                           : baseTurnFactor;

                        clientHardwareEvent.deltaValue =
                            gesture.continuousDelta * turnSpeedFactor;
                    }
                }
            }
            else if (gesture.componentId == REC_GAIN_POT ||
                     gesture.componentId == MAIN_VOLUME_POT)
            {
                if (gesture.type == GestureEvent::Type::UPDATE)
                {
                    clientHardwareEvent.type =
                        ClientHardwareEvent::Type::PotMove;
                    clientHardwareEvent.deltaValue = gesture.continuousDelta;
                }
            }
            break;
        }

        case HostInputEvent::Source::KEYBOARD:
        {
            const auto &[keyDown, rawKeyCode, shiftDown, ctrlDown, altDown] =
                std::get<KeyEvent>(hostInputEvent.payload);
            const auto vmpcKeyCode =
                KeyCodeHelper::getVmpcFromPlatformKeyCode(rawKeyCode);
            const auto binding = keyboardBindings->lookupFirstKeyCodeBinding(vmpcKeyCode);

            if (binding == nullptr || binding->getComponentId() != DATA_WHEEL)
            {
                if (const auto typableChar =
                        KeyCodeHelper::getCharForTypableVmpcKeyCode(
                            vmpcKeyCode);
                    typableChar)
                {
                    char charToUse;

                    if (shiftDown)
                    {
                        const auto charWithShift =
                            KeyCodeHelper::getCharWithShiftModifier(
                                vmpcKeyCode);
                        charToUse = charWithShift.value_or(*typableChar);
                    }
                    else
                    {
                        charToUse = *typableChar;
                    }

                    clientHardwareEvent.textInputKey =
                        ClientHardwareEvent::TextInputKey{charToUse, keyDown};
                }
            }

            if (!binding)
            {
                if (clientHardwareEvent.textInputKey)
                {
                    return ClientEvent{clientHardwareEvent};
                }
                return std::nullopt;
            }

            const auto id = binding->getComponentId();

            if (id == NONE)
            {
                if (clientHardwareEvent.textInputKey)
                {
                    return ClientEvent{clientHardwareEvent};
                }
                return std::nullopt;
            }

            clientHardwareEvent.componentId = id;

            if (id >= PAD_1_OR_AB && id <= PAD_16_OR_PARENTHESES)
            {
                clientHardwareEvent.type =
                    keyDown ? ClientHardwareEvent::Type::PadPress
                            : ClientHardwareEvent::Type::PadRelease;
                clientHardwareEvent.index =
                    static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
                if (keyDown)
                {
                    clientHardwareEvent.value = Pad::MAX_VELO_NORMALIZED;
                }
            }
            else if (id == SLIDER)
            {
                if (!keyDown)
                {
                    break;
                }
                clientHardwareEvent.type =
                    ClientHardwareEvent::Type::SliderMove;
            }
            else if (id == DATA_WHEEL)
            {
                if (!keyDown)
                {
                    break;
                }
                clientHardwareEvent.type =
                    ClientHardwareEvent::Type::DataWheelTurn;

                const auto direction = binding->direction;

                if (direction == Direction::NoDirection)
                {
                    throw std::invalid_argument(
                        "DataWheel bindings must have a Direction");
                }

                int increment = toSign(direction);

                if (ctrlDown)
                {
                    increment *= 10;
                }
                if (altDown)
                {
                    increment *= 10;
                }
                if (shiftDown)
                {
                    increment *= 10;
                }

                clientHardwareEvent.deltaValue = increment;
            }
            else
            {
                clientHardwareEvent.type =
                    keyDown ? ClientHardwareEvent::Type::MpcButtonPress
                            : ClientHardwareEvent::Type::MpcButtonRelease;
            }
            break;
        }

        case HostInputEvent::Source::MIDI:
        default:
            break;
    }

    if (clientHardwareEvent.componentId == NONE &&
        !clientHardwareEvent.textInputKey)
    {
        return std::nullopt;
    }

    if (clientHardwareEvent.deltaValue && clientHardwareEvent.value)
    {
        throw std::runtime_error(
            "ClientHardwareEvent can have either a value, or a deltaValue, not "
            "both.");
    }

    if (clientHardwareEvent.type == ClientHardwareEvent::Type::MpcButtonPress ||
        clientHardwareEvent.type ==
            ClientHardwareEvent::Type::MpcButtonRelease ||
        clientHardwareEvent.type ==
            ClientHardwareEvent::Type::ButtonDoublePress)
    {
        if (clientHardwareEvent.deltaValue || clientHardwareEvent.value)
        {
            throw std::runtime_error(
                "ClientHardwareEvent for Button may not have a deltaValue or "
                "value.");
        }
    }

    if (clientHardwareEvent.type == ClientHardwareEvent::Type::DataWheelTurn &&
        !clientHardwareEvent.deltaValue)
    {
        throw std::runtime_error(
            "ClientHardwareEvent for DataWheel must have a deltaValue.");
    }

    return ClientEvent{clientHardwareEvent};
}
