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

    switch (hostEvent.source){
    case HostInputEvent::MIDI: {
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

    case HostInputEvent::MOUSE:
    {
        const auto& mouse = std::get<MouseEvent>(hostEvent.payload);

        if (mouse.componentId == ComponentId::NONE)
        {
            throw std::invalid_argument("MouseEvent.componentId must not be NONE");
        }

        clientEvent.label = mpc::hardware2::componentIdToLabel.at(mouse.componentId);

        if (mouse.componentId >= ComponentId::PAD1 && mouse.componentId <= ComponentId::PAD16)
        {
            clientEvent.index = static_cast<int>(mouse.componentId) - static_cast<int>(ComponentId::PAD1);
            clientEvent.type =
                (mouse.type == MouseEvent::MouseEventType::BUTTON_DOWN)
                    ? ClientInput::Type::PadPress
                    : (mouse.type == MouseEvent::MouseEventType::BUTTON_UP)
                          ? ClientInput::Type::PadRelease
                          : ClientInput::Type::PadAftertouch;

            if (mouse.type == MouseEvent::MouseEventType::BUTTON_DOWN ||
                mouse.type == MouseEvent::MouseEventType::BUTTON_UP ||
                mouse.type == MouseEvent::MouseEventType::MOVE)
            {
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * static_cast<float>(mpc::hardware2::Pad::MAX_VELO));
            }
        }
        else if (mouse.componentId == ComponentId::SLIDER)
        {
            clientEvent.type = ClientInput::Type::SliderMove;
            clientEvent.value = mouse.normY;
        }
        else if (mouse.componentId >= ComponentId::CURSOR_LEFT && mouse.componentId <= ComponentId::NUM_9)
        {

            if (mouse.type == MouseEvent::MouseEventType::BUTTON_DOWN)
            {
                clientEvent.type = ClientInput::Type::ButtonPress;
            }
            else if (mouse.type == MouseEvent::MouseEventType::BUTTON_UP)
            {
                clientEvent.type = ClientInput::Type::ButtonRelease;
            }
            else
            {
                return std::nullopt;
            }
        }
        else if (mouse.componentId == ComponentId::DATA_WHEEL)
        {
            clientEvent.type = ClientInput::Type::DataWheelTurn;

            if (mouse.type == MouseEvent::MouseEventType::WHEEL)
            {
                clientEvent.value = static_cast<int>(mouse.wheelDelta);
            }
            else if (mouse.type == MouseEvent::MouseEventType::MOVE || mouse.type == MouseEvent::MouseEventType::DRAG)
            {
                clientEvent.value = static_cast<int>(mouse.deltaY);
            }
        }
        else if (mouse.componentId == ComponentId::NONE)
        {
            throw std::invalid_argument("MouseEvent.componentId must not be NONE");
        }
        break;
    }

    case HostInputEvent::KEYBOARD:
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

