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
    ClientInput clientEvent;

    switch (hostEvent.getSource())
    {
    case HostInputEvent::Source::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostEvent.payload);
        if (midi.messageType == MidiEvent::NOTE)
        {
            int padIndex = midi.data1 % 16;
            if (midi.data2 > 0)
            {
                clientEvent.type = ClientInput::Type::PadPress;
                clientEvent.index = padIndex;
                clientEvent.value = midi.data2;
            }
            else
            {
                clientEvent.type = ClientInput::Type::PadRelease;
                clientEvent.index = padIndex;
            }
        }
        else if (midi.messageType == MidiEvent::CC)
        {
            clientEvent.type = ClientInput::Type::SliderMove;
            clientEvent.value = midi.data2;
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

        clientEvent.label = mpc::hardware2::componentIdToLabel.at(gesture.componentId);

        if (gesture.componentId >= ComponentId::PAD1 && gesture.componentId <= ComponentId::PAD16)
        {
            if (gesture.type == GestureEvent::Type::BEGIN || gesture.type == GestureEvent::Type::END)
            {
                clientEvent.index = static_cast<int>(gesture.componentId) - static_cast<int>(ComponentId::PAD1);
                clientEvent.type = gesture.type == GestureEvent::Type::BEGIN ?
                    ClientInput::Type::PadPress : ClientInput::Type::PadRelease;

                if (gesture.type == GestureEvent::Type::BEGIN)
                {
                    clientEvent.value = static_cast<int>((1.f - gesture.normY) * static_cast<float>(mpc::hardware2::Pad::MAX_VELO));
                }
            }
        }
        else if (gesture.componentId == ComponentId::SLIDER && gesture.componentId)
        {
            if (gesture.type == GestureEvent::Type::UPDATE)
            {
                clientEvent.type = ClientInput::Type::SliderMove;
                clientEvent.value = gesture.normY;
            }
        }
        else if (gesture.componentId >= ComponentId::CURSOR_LEFT && gesture.componentId <= ComponentId::NUM_9)
        {
            if (gesture.type == GestureEvent::Type::BEGIN)
            {
                if (gesture.repeatCount == 2)
                {
                    printf("gesture has repeat count of 2\n");
                    clientEvent.type = ClientInput::Type::ButtonDoublePress;
                }
                else
                {
                    clientEvent.type = ClientInput::Type::ButtonPress;
                }
            }
            else if (gesture.type == GestureEvent::Type::END)
            {
                clientEvent.type = ClientInput::Type::ButtonRelease;
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
                clientEvent.type = ClientInput::Type::DataWheelTurn;
                clientEvent.value = gesture.stepDelta;
            }
        }
        else if (gesture.componentId == ComponentId::NONE)
        {
            throw std::invalid_argument("GestureEvent.componentId must not be NONE");
        }
        break;
    }

    case HostInputEvent::Source::KEYBOARD:
    {
        const auto& key = std::get<KeyEvent>(hostEvent.payload);
        const auto vmpcKeyCode = controls::KeyCodeHelper::getVmpcFromPlatformKeyCode(key.rawKeyCode);
        const auto label = kbMapping->getHardwareComponentLabelAssociatedWithKeycode(vmpcKeyCode);

        clientEvent.label = label;

        if (label.substr(0, 4) == "pad-")
        {
            const auto digitsString = label.substr(4);
            const auto padNumber = std::stoi(digitsString);
            clientEvent.type = key.keyDown ? ClientInput::Type::PadPress : ClientInput::Type::PadRelease;
            clientEvent.index = padNumber - 1;
            if (key.keyDown) clientEvent.value = mpc::hardware2::Pad::MAX_VELO;
        }
        else if (label == "slider")
        {
            if (!key.keyDown) break;
            clientEvent.type = ClientInput::Type::SliderMove;
        }
        else if (label.substr(0, std::string("datawheel-").length()) == "datawheel-")
        {
            if (!key.keyDown) break;
            clientEvent.type = ClientInput::Type::DataWheelTurn;

            int increment = 1;

            if (key.ctrlDown) increment *= 10;
            if (key.altDown) increment *= 10;
            if (key.shiftDown) increment *= 10;

            if (label.find("down") != std::string::npos)
            {
                increment = -increment;
            }

            clientEvent.value = increment;
        }
        else
        {
            clientEvent.type = key.keyDown ? ClientInput::Type::ButtonPress : ClientInput::Type::ButtonRelease;
        }
        break;
    }
    }

    return clientEvent;
}

