#include "input/HostToClientTranslator.h"
#include "hardware/ComponentId.h"
#include "input/ClientInput.h"
#include "input/KeyboardBindings.h"

#include "input/KeyCodeHelper.hpp"

#include "hardware/HardwareComponent.h"

#include <stdexcept>

using namespace mpc::input;
using namespace mpc::hardware;

std::optional<ClientInput> HostToClientTranslator::translate(const HostInputEvent& hostInputEvent, std::shared_ptr<KeyboardBindings> keyboardBindings)
{
    ClientInput clientInput;
    
    switch (hostInputEvent.getSource())
    {
        case HostInputEvent::Source::MIDI:
            clientInput.source = ClientInput::Source::HostInputMidi;
            break;
        case HostInputEvent::Source::KEYBOARD:
            clientInput.source = ClientInput::Source::HostInputKeyboard;
            break;
        case HostInputEvent::Source::GESTURE:
            clientInput.source = ClientInput::Source::HostInputGesture;
            break;
        case HostInputEvent::Source::FOCUS:
            clientInput.source = ClientInput::Source::HostFocusEvent;
            break;
    }

    switch (hostInputEvent.getSource())
    {
    case HostInputEvent::Source::FOCUS:
        clientInput.type = ClientInput::Type::HostFocusLost;
        break;
    case HostInputEvent::Source::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostInputEvent.payload);
        if (midi.messageType == MidiEvent::NOTE)
        {
            int padIndex = midi.data1 % 16;
            if (midi.data2 > 0)
            {
                clientInput.type = ClientInput::Type::PadPress;
                clientInput.index = padIndex;
                clientInput.value = midi.data2;
            }
            else
            {
                clientInput.type = ClientInput::Type::PadRelease;
                clientInput.index = padIndex;
            }
        }
        else if (midi.messageType == MidiEvent::CC)
        {
            clientInput.type = ClientInput::Type::SliderMove;
            clientInput.value = midi.data2;
        }
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
                clientInput.type = ClientInput::Type::PadPress;
                clientInput.value = 1.f - gesture.normY;
            }
            else if (gesture.type == GestureEvent::Type::END)
            {
                clientInput.type = ClientInput::Type::PadRelease;
            }
            else if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientInput::Type::PadAftertouch;
                clientInput.value = 1.f - gesture.normY;
            }
        }
        else if (gesture.componentId == ComponentId::SLIDER)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientInput::Type::SliderMove;

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
                if (gesture.repeatCount == 2)
                {
                    clientInput.type = ClientInput::Type::ButtonDoublePress;
                }
            }
            else if (gesture.type == GestureEvent::Type::BEGIN)
            {
                if (gesture.repeatCount == 2)
                {
                    clientInput.type = ClientInput::Type::ButtonDoublePress;
                }
                else
                {
                    clientInput.type = ClientInput::Type::ButtonPress;
                }
            }
            else if (gesture.type == GestureEvent::Type::END)
            {
                clientInput.type = ClientInput::Type::ButtonRelease;
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
                clientInput.type = ClientInput::Type::DataWheelTurn;
                clientInput.deltaValue = gesture.continuousDelta;
            }
        }
        else if (gesture.componentId == ComponentId::REC_GAIN_POT ||
                 gesture.componentId == ComponentId::MAIN_VOLUME_POT)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientInput.type = ClientInput::Type::PotMove;
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

        if (!binding)
        {
            return std::nullopt;
        }

        const auto id = binding->componentId;

        if (id == hardware::ComponentId::NONE)
        {
            return std::nullopt;
        }
        
        clientInput.componentId = id;

        if (id >= ComponentId::PAD_1_OR_AB && id <= ComponentId::PAD_16_OR_PARENTHESES)
        {
            clientInput.type = key.keyDown ? ClientInput::Type::PadPress : ClientInput::Type::PadRelease;
            clientInput.index = static_cast<int>(id) - static_cast<int>(ComponentId::PAD_1_OR_AB);
            if (key.keyDown) clientInput.value = mpc::hardware::Pad::MAX_VELO;
        }
        else if (id == ComponentId::SLIDER)
        {
            if (!key.keyDown) break;
            clientInput.type = ClientInput::Type::SliderMove;
        }
        else if (id == ComponentId::DATA_WHEEL)
        {
            if (!key.keyDown) break;
            clientInput.type = ClientInput::Type::DataWheelTurn;

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
            clientInput.type = key.keyDown ? ClientInput::Type::ButtonPress : ClientInput::Type::ButtonRelease;
        }
        break;
    }
    }

    if (clientInput.componentId == ComponentId::NONE)
    {
        throw std::runtime_error("ClientInput must have a ComponentId.");
    }

    if (clientInput.deltaValue && clientInput.value)
    {
        throw std::runtime_error("ClientInput can have either a value, or a deltaValue, not both.");
    }

    if (clientInput.type == ClientInput::Type::ButtonPress || clientInput.type == ClientInput::Type::ButtonRelease || clientInput.type == ClientInput::Type::ButtonDoublePress)
    {
        if (clientInput.deltaValue || clientInput.value)
        {
            throw std::runtime_error("ClientInput for Button may not have a deltaValue or value.");
        }
    }

    if (clientInput.type == ClientInput::Type::DataWheelTurn && !clientInput.deltaValue)
    {
        throw std::runtime_error("ClientInput for DataWheel must have a deltaValue.");
    }

    return clientInput;
}

