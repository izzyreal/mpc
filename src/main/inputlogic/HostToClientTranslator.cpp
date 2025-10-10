#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/ClientInput.h"

#include <stdexcept>

using namespace mpc::inputlogic;

ClientInput HostToClientTranslator::translate(const HostInputEvent& hostEvent) {
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
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * 127.f);
        }

        // Handle regular buttons
        else if (mouse.guiElement >= MouseEvent::CURSOR_LEFT && mouse.guiElement <= MouseEvent::NUM_9) {
            // Map enum → internal button label
            switch (mouse.guiElement) {
                case MouseEvent::PLAY:        clientEvent.label = "play"; break;
                case MouseEvent::STOP:        clientEvent.label = "stop"; break;
                case MouseEvent::REC:         clientEvent.label = "rec"; break;
                case MouseEvent::OVERDUB:     clientEvent.label = "overdub"; break;
                case MouseEvent::PLAY_START:  clientEvent.label = "play-start"; break;
                case MouseEvent::MAIN_SCREEN: clientEvent.label = "main-screen"; break;
                case MouseEvent::SHIFT:       clientEvent.label = "shift"; break;
                default:                      clientEvent.label = "button"; break;
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

    case HostInputEvent::KEYBOARD: {
        const auto& key = std::get<KeyEvent>(hostEvent.payload);
        if (key.rawKeyCode == 32) { // Spacebar
            clientEvent.label = "play";
            clientEvent.type = key.keyDown
                                   ? ClientInput::Type::ButtonPress
                                   : ClientInput::Type::ButtonRelease;
        }
        break;
    }
    }

    return clientEvent;
}

