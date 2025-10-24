#include "input/HostToClientTranslator.hpp"
#include "hardware/ComponentId.hpp"
#include "input/ClientHardwareEvent.hpp"
#include "input/KeyboardBindings.hpp"

#include "input/KeyCodeHelper.hpp"

#include "hardware/Component.hpp"

#include <stdexcept>

using namespace mpc::input;
using namespace mpc::hardware;

std::optional<ClientHardwareEvent> HostToClientTranslator::translate(const HostInputEvent& hostInputEvent, std::shared_ptr<KeyboardBindings> keyboardBindings)
{
    ClientHardwareEvent clientInput;
    
    switch (hostInputEvent.getSource())
    {
        case HostInputEvent::Source::MIDI:
            clientInput.source = ClientHardwareEvent::Source::HostInputMidi;
            break;
        case HostInputEvent::Source::KEYBOARD:
            clientInput.source = ClientHardwareEvent::Source::HostInputKeyboard;
            break;
        case HostInputEvent::Source::GESTURE:
            clientInput.source = ClientHardwareEvent::Source::HostInputGesture;
            break;
        case HostInputEvent::Source::FOCUS:
            clientInput.source = ClientHardwareEvent::Source::HostFocusEvent;
            break;
    }

    switch (hostInputEvent.getSource())
    {
    case HostInputEvent::Source::FOCUS:
        clientInput.type = ClientHardwareEvent::Type::HostFocusLost;
        break;
    case HostInputEvent::Source::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostInputEvent.payload);
        midi.printInfo();
        break;
    }

    case HostInputEvent::Source::GESTURE:
    {
        const auto& gesture = std::get<GestureEvent>(hostInputEvent.payload);

        if (gesture.componentId == ComponentId::NONE)
        {
            throw std::invalid_argument("GestureEvent.componentId must not be NONE");
        }

        clientInput.componentId = gesture.componentId;

        if (gesture.componentId >= ComponentId::PAD_1_OR_AB && gesture.componentId <= ComponentId::PAD_16_OR_PARENTHESES)
        {
            clientInput.index = static_cast<int>(gesture.componentId) - static_cast<int>(ComponentId::PAD_1_OR_AB);

            if (gesture.type == GestureEvent::Type::BEGIN || gesture.type == GestureEvent::Type::REPEAT)
            {
                clientInput.type = ClientHardwareEvent::Type::PadPress;
                clientInput.value = 1.f - gesture.normY;
            }
            else if (gesture.type == GestureEvent::Type::END)
            {
                clientInput.type = ClientHardwareEvent::Type::PadRelease;
            }
            else if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientHardwareEvent::Type::PadAftertouch;
                clientInput.value = 1.f - gesture.normY;
            }
        }
        else if (gesture.componentId == ComponentId::SLIDER)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientHardwareEvent::Type::SliderMove;

                if (gesture.movement == GestureEvent::Movement::Relative)
                {
                    clientInput.deltaValue = gesture.continuousDelta;
                }
                else /* Movement::Absolute */
                {
                    clientInput.value = gesture.normY;
                }
            }
        }
        else if (gesture.componentId >= ComponentId::CURSOR_LEFT_OR_DIGIT && gesture.componentId <= ComponentId::NUM_9_OR_MIDI_SYNC)
        {
            if (gesture.type == GestureEvent::Type::REPEAT)
            {
                if (gesture.repeatCount >= 2)
                {
                    clientInput.type = ClientHardwareEvent::Type::ButtonDoublePress;
                }
            }
            else if (gesture.type == GestureEvent::Type::BEGIN)
            {
                if (gesture.repeatCount >= 2)
                {
                    clientInput.type = ClientHardwareEvent::Type::ButtonDoublePress;
                }
                else
                {
                    clientInput.type = ClientHardwareEvent::Type::ButtonPress;
                }
            }
            else if (gesture.type == GestureEvent::Type::END)
            {
                clientInput.type = ClientHardwareEvent::Type::ButtonRelease;
            }
            else
            {
                return std::nullopt;
            }
        }
        else if (gesture.componentId == ComponentId::DATA_WHEEL)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientHardwareEvent::Type::DataWheelTurn;
                clientInput.deltaValue = gesture.continuousDelta;
            }
        }
        else if (gesture.componentId == ComponentId::REC_GAIN_POT ||
                 gesture.componentId == ComponentId::MAIN_VOLUME_POT)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientHardwareEvent::Type::PotMove;
                clientInput.deltaValue = gesture.continuousDelta;
            }
        }
        break;
    }

    case HostInputEvent::Source::KEYBOARD:
    {
        const auto &key = std::get<KeyEvent>(hostInputEvent.payload);
        const auto vmpcKeyCode = KeyCodeHelper::getVmpcFromPlatformKeyCode(key.rawKeyCode);
        const auto binding = keyboardBindings->lookup(vmpcKeyCode);

        if (!binding || binding->componentId != ComponentId::DATA_WHEEL)
        {
            if (const auto typableChar = KeyCodeHelper::getCharForTypableVmpcKeyCode(vmpcKeyCode); typableChar)
            {
                char charToUse;

                if (key.shiftDown)
                {
                    const auto charWithShift = KeyCodeHelper::getCharWithShiftModifier(vmpcKeyCode);
                    charToUse = charWithShift.value_or(*typableChar);
                }
                else
                {
                    charToUse = *typableChar;
                }

                clientInput.textInputKey = ClientHardwareEvent::TextInputKey { charToUse, key.keyDown };
            }
        }

        if (!binding)
        {
            if (clientInput.textInputKey)
            {
                return clientInput;
            }
            else
            {
                return std::nullopt;
            }
        }

        const auto id = binding->componentId;

        if (id == hardware::ComponentId::NONE)
        {
            if (clientInput.textInputKey)
            {
                return clientInput;
            }
            else
            {
                return std::nullopt;
            }
        }
        
        clientInput.componentId = id;

        if (id >= ComponentId::PAD_1_OR_AB && id <= ComponentId::PAD_16_OR_PARENTHESES)
        {
            clientInput.type = key.keyDown ? ClientHardwareEvent::Type::PadPress : ClientHardwareEvent::Type::PadRelease;
            clientInput.index = static_cast<int>(id) - static_cast<int>(ComponentId::PAD_1_OR_AB);
            if (key.keyDown) clientInput.value = mpc::hardware::Pad::MAX_VELO;
        }
        else if (id == ComponentId::SLIDER)
        {
            if (!key.keyDown) break;
            clientInput.type = ClientHardwareEvent::Type::SliderMove;
        }
        else if (id == ComponentId::DATA_WHEEL)
        {
            if (!key.keyDown) break;
            clientInput.type = ClientHardwareEvent::Type::DataWheelTurn;

            const auto direction = binding->direction;

            if (direction == Direction::None)
            {
                std::invalid_argument("DataWheel bindings must have a Direction");
            }

            int increment = toSign(direction);

            if (key.ctrlDown) increment *= 10;
            if (key.altDown) increment *= 10;
            if (key.shiftDown) increment *= 10;

            clientInput.deltaValue = increment;
        }
        else
        {
            clientInput.type = key.keyDown ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        break;
    }
    }

    if (clientInput.componentId == ComponentId::NONE && !clientInput.textInputKey)
    {
        return std::nullopt;
    }

    if (clientInput.deltaValue && clientInput.value)
    {
        throw std::runtime_error("ClientHardwareEvent can have either a value, or a deltaValue, not both.");
    }

    if (clientInput.type == ClientHardwareEvent::Type::ButtonPress || clientInput.type == ClientHardwareEvent::Type::ButtonRelease || clientInput.type == ClientHardwareEvent::Type::ButtonDoublePress)
    {
        if (clientInput.deltaValue || clientInput.value)
        {
            throw std::runtime_error("ClientHardwareEvent for Button may not have a deltaValue or value.");
        }
    }

    if (clientInput.type == ClientHardwareEvent::Type::DataWheelTurn && !clientInput.deltaValue)
    {
        throw std::runtime_error("ClientHardwareEvent for DataWheel must have a deltaValue.");
    }

    return clientInput;
}

