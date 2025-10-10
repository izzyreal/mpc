#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/ClientInput.h"

#include "controls/KeyCodeHelper.hpp"
#include "controls/KbMapping.hpp"

#include "hardware2/HardwareComponent.h"

#include <stdexcept>

using namespace mpc::inputlogic;

ClientInput HostToClientTranslator::translate(const HostInputEvent& hostEvent, std::shared_ptr<mpc::controls::KbMapping> kbMapping)
{
    ClientInput clientEvent;

    switch (hostEvent.source) {
    case HostInputEvent::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostEvent.payload);
        if (midi.messageType == MidiEvent::NOTE) {
            int padIndex = midi.data1 % 16;
            if (midi.data2 > 0) {
                clientEvent.type = ClientInput::Type::PadPress;
                clientEvent.index = padIndex;
                clientEvent.value = midi.data2;
            } else {
                clientEvent.type = ClientInput::Type::PadRelease;
                clientEvent.index = padIndex;
            }
        } else if (midi.messageType == MidiEvent::CC) {
            clientEvent.type = ClientInput::Type::SliderMove;
            clientEvent.value = midi.data2;
        }
        break;
    }

    case HostInputEvent::MOUSE: {
        const auto& mouse = std::get<MouseEvent>(hostEvent.payload);

        // Handle pads (PAD1–PAD16)
        if (mouse.guiElement >= MouseEvent::PAD1 && mouse.guiElement <= MouseEvent::PAD16) {
            clientEvent.index = static_cast<int>(mouse.guiElement) - static_cast<int>(MouseEvent::PAD1);
            clientEvent.type =
                (mouse.type == MouseEvent::BUTTON_DOWN)
                    ? ClientInput::Type::PadPress
                    : (mouse.type == MouseEvent::BUTTON_UP)
                          ? ClientInput::Type::PadRelease
                          : ClientInput::Type::PadAftertouch;

            if (mouse.type == MouseEvent::BUTTON_DOWN ||
                mouse.type == MouseEvent::BUTTON_UP ||
                mouse.type == MouseEvent::MOVE)
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * static_cast<float>(mpc::hardware2::Pad::MAX_VELO));
        }

        // Handle regular buttons
        else if (mouse.guiElement >= MouseEvent::CURSOR_LEFT && mouse.guiElement <= MouseEvent::NUM_9) {
            switch (mouse.guiElement) {
                case MouseEvent::CURSOR_LEFT:       clientEvent.label = "left"; break;
                case MouseEvent::CURSOR_RIGHT:      clientEvent.label = "right"; break;
                case MouseEvent::CURSOR_UP:         clientEvent.label = "up"; break;
                case MouseEvent::CURSOR_DOWN:       clientEvent.label = "down"; break;
                case MouseEvent::REC:               clientEvent.label = "rec"; break;
                case MouseEvent::OVERDUB:           clientEvent.label = "overdub"; break;
                case MouseEvent::STOP:              clientEvent.label = "stop"; break;
                case MouseEvent::PLAY:              clientEvent.label = "play"; break;
                case MouseEvent::PLAY_START:        clientEvent.label = "play-start"; break;
                case MouseEvent::MAIN_SCREEN:       clientEvent.label = "main-screen"; break;
                case MouseEvent::PREV_STEP_EVENT:   clientEvent.label = "prev-step-event"; break;
                case MouseEvent::NEXT_STEP_EVENT:   clientEvent.label = "next-step-event"; break;
                case MouseEvent::GO_TO:             clientEvent.label = "go-to"; break;
                case MouseEvent::PREV_BAR_START:    clientEvent.label = "prev-bar-start"; break;
                case MouseEvent::NEXT_BAR_END:      clientEvent.label = "next-bar-end"; break;
                case MouseEvent::TAP:               clientEvent.label = "tap"; break;
                case MouseEvent::NEXT_SEQ:          clientEvent.label = "next-seq"; break;
                case MouseEvent::TRACK_MUTE:        clientEvent.label = "track-mute"; break;
                case MouseEvent::OPEN_WINDOW:       clientEvent.label = "open-window"; break;
                case MouseEvent::FULL_LEVEL:        clientEvent.label = "full-level"; break;
                case MouseEvent::SIXTEEN_LEVELS:    clientEvent.label = "sixteen-levels"; break;
                case MouseEvent::F1:                clientEvent.label = "f1"; break;
                case MouseEvent::F2:                clientEvent.label = "f2"; break;
                case MouseEvent::F3:                clientEvent.label = "f3"; break;
                case MouseEvent::F4:                clientEvent.label = "f4"; break;
                case MouseEvent::F5:                clientEvent.label = "f5"; break;
                case MouseEvent::F6:                clientEvent.label = "f6"; break;
                case MouseEvent::SHIFT:             clientEvent.label = "shift"; break;
                case MouseEvent::ENTER:             clientEvent.label = "enter"; break;
                case MouseEvent::UNDO_SEQ:          clientEvent.label = "undo-seq"; break;
                case MouseEvent::ERASE:             clientEvent.label = "erase"; break;
                case MouseEvent::AFTER:             clientEvent.label = "after"; break;
                case MouseEvent::BANK_A:            clientEvent.label = "bank-a"; break;
                case MouseEvent::BANK_B:            clientEvent.label = "bank-b"; break;
                case MouseEvent::BANK_C:            clientEvent.label = "bank-c"; break;
                case MouseEvent::BANK_D:            clientEvent.label = "bank-d"; break;
                case MouseEvent::NUM_0:             clientEvent.label = "0"; break;
                case MouseEvent::NUM_1:             clientEvent.label = "1"; break;
                case MouseEvent::NUM_2:             clientEvent.label = "2"; break;
                case MouseEvent::NUM_3:             clientEvent.label = "3"; break;
                case MouseEvent::NUM_4:             clientEvent.label = "4"; break;
                case MouseEvent::NUM_5:             clientEvent.label = "5"; break;
                case MouseEvent::NUM_6:             clientEvent.label = "6"; break;
                case MouseEvent::NUM_7:             clientEvent.label = "7"; break;
                case MouseEvent::NUM_8:             clientEvent.label = "8"; break;
                case MouseEvent::NUM_9:             clientEvent.label = "9"; break;

                case MouseEvent::NONE:
                    throw std::invalid_argument("MouseEvent.guiElement must not be NONE");

                default:
                    // If we ever add new GUI elements, default to a generic label
                    clientEvent.label = "button";
                    break;
            }
            if (mouse.type == MouseEvent::BUTTON_DOWN)
                clientEvent.type = ClientInput::Type::ButtonPress;
            else if (mouse.type == MouseEvent::BUTTON_UP)
                clientEvent.type = ClientInput::Type::ButtonRelease;
            else
                return clientEvent; // Ignore non-button events for buttons
        }

        // Handle data wheel
        else if (mouse.guiElement == MouseEvent::DATA_WHEEL) {
            clientEvent.type = ClientInput::Type::DataWheelTurn;
            if (mouse.type == MouseEvent::WHEEL)
                clientEvent.value = static_cast<int>(mouse.wheelDelta);
            else if (mouse.type == MouseEvent::MOVE || mouse.type == MouseEvent::DRAG)
                clientEvent.value = static_cast<int>(mouse.deltaY);
            else
                clientEvent.value = 0;
        }

        else if (mouse.guiElement == MouseEvent::NONE) {
            throw std::invalid_argument("MouseEvent.guiElement must not be NONE");
        }

        // Fallback for scroll wheel events not tied to DATA_WHEEL
        if (mouse.guiElement != MouseEvent::DATA_WHEEL && mouse.type == MouseEvent::WHEEL) {
            clientEvent.type = ClientInput::Type::DataWheelTurn;
            clientEvent.value = static_cast<int>(mouse.wheelDelta);
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

