#pragma once

#include <variant>
#include <cstdint>

#include "hardware2/ComponentIdLabelMap.h"

namespace mpc::inputlogic {

struct KeyEvent {
    bool keyDown;
    int rawKeyCode;
    bool shiftDown;
    bool ctrlDown;
    bool altDown;
};

struct MouseEvent {

    struct ButtonState {
        bool left;
        bool middle;
        bool right;
    };
    enum MouseEventType {
        MOVE,
        DRAG,
        WHEEL,
        BUTTON_DOWN,
        BUTTON_UP
    };

    ButtonState buttonState;
    hardware2::ComponentId componentId;
    
    float normX = 0.0f;
    float normY = 0.0f;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    float wheelDelta = 0.0f;
    MouseEventType type;
};

struct MidiEvent {
    enum MessageType { CC, NOTE };
    MessageType messageType;
    uint8_t statusByte;
    uint8_t data1;
    uint8_t data2;
};

/* Input events coming from the host, i.e. the machine the virtual MPC2000XL is virtualized on */
struct HostInputEvent {
    enum Source { KEYBOARD, MOUSE, MIDI };
    Source source;
    std::variant<KeyEvent, MouseEvent, MidiEvent> payload;
};

} // namespace mpc::inputlogic
