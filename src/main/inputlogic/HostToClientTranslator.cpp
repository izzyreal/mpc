#include "HostToClientTranslator.h"
#include <stdexcept>

using namespace mpc::inputlogic;

ClientInputEvent HostToClientTranslator::translate(const HostInputEvent& hostEvent) {
    ClientInputEvent clientEvent;

    switch (hostEvent.source) {
    case HostInputEvent::MIDI: {
        const auto& midi = std::get<MidiEvent>(hostEvent.payload);
        if (midi.messageType == MidiEvent::NOTE) {
            int padIndex = midi.data1 % 16;
            if (midi.data2 > 0) {
                clientEvent.component = "pad-" + std::to_string(padIndex);
                clientEvent.action = "press";
                clientEvent.value = midi.data2;
            } else {
                clientEvent.component = "pad-" + std::to_string(padIndex);
                clientEvent.action = "release";
            }
        } else if (midi.messageType == MidiEvent::CC) {
            clientEvent.component = "slider";
            clientEvent.action = "move";
            clientEvent.value = midi.data2;
        }
        break;
    }
    case HostInputEvent::MOUSE: {
        const auto& mouse = std::get<MouseEvent>(hostEvent.payload);
        switch (mouse.guiElement) {
        case MouseEvent::PAD1:
            clientEvent.component = "pad-0";
            break;
        case MouseEvent::PAD2:
            clientEvent.component = "pad-1";
            break;
        case MouseEvent::PAD3:
            clientEvent.component = "pad-2";
            break;
        case MouseEvent::PLAY_BUTTON:
            clientEvent.component = "play";
            break;
        case MouseEvent::STOP_BUTTON:
            clientEvent.component = "stop";
            break;
        case MouseEvent::NONE:
            throw std::invalid_argument("guiElement must be provided\n");
        }
        if (mouse.type == MouseEvent::DOWN) clientEvent.action = "press";
        else if (mouse.type == MouseEvent::UP) clientEvent.action = "release";
        else if (mouse.type == MouseEvent::WHEEL) {
            clientEvent.component = "datawheel";
            clientEvent.action = mouse.wheelDelta > 0 ? "turn-up" : "turn-down";
        }
        break;

    }
    case HostInputEvent::KEYBOARD: {
        const auto& key = std::get<KeyEvent>(hostEvent.payload);
        if (key.rawKeyCode == 32) {
            clientEvent.component = "play";
            clientEvent.action = key.keyDown ? "press" : "release";
        }
        break;
    }
    }

    return clientEvent;
}
