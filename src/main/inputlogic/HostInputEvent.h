#pragma once
#include <variant>
#include <cstdint>

namespace mpc::inputlogic {

struct KeyEvent {
    bool keyDown;
    int rawKeyCode;
};

struct MouseEvent {
    enum GuiElement {
        NONE, PAD1, PAD2, PAD3, PLAY_BUTTON, STOP_BUTTON
    };
    struct ButtonState {
        bool left;
        bool middle;
        bool right;
    };
    enum MouseEventType {
        MOVE,
        WHEEL,
        DOWN,
        UP
    };
    ButtonState buttonState;
    GuiElement guiElement;
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
