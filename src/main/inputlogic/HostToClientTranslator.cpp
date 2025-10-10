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

        switch (mouse.guiElement) {
        case MouseEvent::PAD1:
            clientEvent.index = 0;
            clientEvent.type = (mouse.type == MouseEvent::DOWN)
                                   ? ClientInput::Type::PadPress
                                   : (mouse.type == MouseEvent::UP)
                                         ? ClientInput::Type::PadRelease
                                         : ClientInput::Type::PadAftertouch;
            if (mouse.type == MouseEvent::DOWN || mouse.type == MouseEvent::UP || mouse.type == MouseEvent::MOVE)
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * 127.f);
            break;

        case MouseEvent::PAD2:
            clientEvent.index = 1;
            clientEvent.type = (mouse.type == MouseEvent::DOWN)
                                   ? ClientInput::Type::PadPress
                                   : (mouse.type == MouseEvent::UP)
                                         ? ClientInput::Type::PadRelease
                                         : ClientInput::Type::PadAftertouch;
            if (mouse.type == MouseEvent::DOWN || mouse.type == MouseEvent::UP || mouse.type == MouseEvent::MOVE)
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * 127.f);
            break;

        case MouseEvent::PAD3:
            clientEvent.index = 2;
            clientEvent.type = (mouse.type == MouseEvent::DOWN)
                                   ? ClientInput::Type::PadPress
                                   : (mouse.type == MouseEvent::UP)
                                         ? ClientInput::Type::PadRelease
                                         : ClientInput::Type::PadAftertouch;
            if (mouse.type == MouseEvent::DOWN || mouse.type == MouseEvent::UP || mouse.type == MouseEvent::MOVE)
                clientEvent.value = static_cast<int>((1.f - mouse.normY) * 127.f);
            break;

        case MouseEvent::PLAY_BUTTON:
            clientEvent.label = "play";
            clientEvent.type = (mouse.type == MouseEvent::DOWN)
                                   ? ClientInput::Type::ButtonPress
                                   : ClientInput::Type::ButtonRelease;
            break;

        case MouseEvent::STOP_BUTTON:
            clientEvent.label = "stop";
            clientEvent.type = (mouse.type == MouseEvent::DOWN)
                                   ? ClientInput::Type::ButtonPress
                                   : ClientInput::Type::ButtonRelease;
            break;

        case MouseEvent::DATA_WHEEL:
            // --- Handle all DataWheel GUI events here ---
            clientEvent.type = ClientInput::Type::DataWheelTurn;
            if (mouse.type == MouseEvent::WHEEL)
                clientEvent.value = static_cast<int>(mouse.wheelDelta);
            else if (mouse.type == MouseEvent::MOVE || mouse.type == MouseEvent::DRAG)
                clientEvent.value = static_cast<int>(mouse.deltaY);
            else
                clientEvent.value = 0;
            break;

        case MouseEvent::NONE:
            throw std::invalid_argument("guiElement must be provided\n");
        }

        // Wheel fallback (for other GUI elements)
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
            clientEvent.type = key.keyDown ? ClientInput::Type::ButtonPress
                                           : ClientInput::Type::ButtonRelease;
        }
        break;
    }
    }

    return clientEvent;
}

