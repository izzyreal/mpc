#include "inputlogic/HostToClientTranslator.h"
#include "hardware2/ComponentIdLabelMap.h"
#include "inputlogic/ClientInput.h"

#include "controls/KeyCodeHelper.hpp"
#include "controls/KbMapping.hpp"

#include "hardware2/HardwareComponent.h"

#include <stdexcept>

using namespace mpc::inputlogic;
using namespace mpc::hardware2;

std::optional<ClientInput> HostToClientTranslator::translate(const HostInputEvent& hostEvent, std::shared_ptr<mpc::controls::KbMapping> kbMapping)
{
    ClientInput clientInput;

    switch (hostEvent.getSource())
    {
    case HostInputEvent::Source::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostEvent.payload);
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
        const auto& gesture = std::get<GestureEvent>(hostEvent.payload);

        if (gesture.componentId == ComponentId::NONE)
        {
            throw std::invalid_argument("GestureEvent.componentId must not be NONE");
        }

        clientInput.label = mpc::hardware2::componentIdToLabel.at(gesture.componentId);

        if (gesture.componentId >= ComponentId::PAD1 && gesture.componentId <= ComponentId::PAD16)
        {
            clientInput.index = static_cast<int>(gesture.componentId) - static_cast<int>(ComponentId::PAD1);

            if (gesture.type == GestureEvent::Type::BEGIN)
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
        else if (gesture.componentId >= ComponentId::CURSOR_LEFT && gesture.componentId <= ComponentId::NUM_9)
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
        const auto& key = std::get<KeyEvent>(hostEvent.payload);
        const auto vmpcKeyCode = controls::KeyCodeHelper::getVmpcFromPlatformKeyCode(key.rawKeyCode);
        const auto label = kbMapping->getHardwareComponentLabelAssociatedWithKeycode(vmpcKeyCode);

        if (label.empty())
        {
            return std::nullopt;
        }
        
        clientInput.label = label;

        if (label.substr(0, 4) == "pad-")
        {
            const auto digitsString = label.substr(4);
            const auto padNumber = std::stoi(digitsString);
            clientInput.type = key.keyDown ? ClientInput::Type::PadPress : ClientInput::Type::PadRelease;
            clientInput.index = padNumber - 1;
            if (key.keyDown) clientInput.value = mpc::hardware2::Pad::MAX_VELO;
        }
        else if (label == "slider")
        {
            if (!key.keyDown) break;
            clientInput.type = ClientInput::Type::SliderMove;
        }
        else if (label.substr(0, std::string("datawheel-").length()) == "datawheel-")
        {
            if (!key.keyDown) break;
            clientInput.type = ClientInput::Type::DataWheelTurn;

            int increment = 1;

            if (key.ctrlDown) increment *= 10;
            if (key.altDown) increment *= 10;
            if (key.shiftDown) increment *= 10;

            if (label.find("down") != std::string::npos)
            {
                increment = -increment;
            }

            clientInput.deltaValue = increment;
        }
        else
        {
            clientInput.type = key.keyDown ? ClientInput::Type::ButtonPress : ClientInput::Type::ButtonRelease;
        }
        break;
    }
    }

    if (!clientInput.label)
    {
        throw std::runtime_error("ClientInput must have a label.");
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

