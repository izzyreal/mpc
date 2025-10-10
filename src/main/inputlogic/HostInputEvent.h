#pragma once
#include <variant>
#include <cstdint>

namespace mpc::inputlogic {

struct KeyEvent {
    bool keyDown;
    int rawKeyCode;
    bool shiftDown;
    bool ctrlDown;
    bool altDown;
};

struct MouseEvent {
    enum GuiElement {
        NONE,

        // Pads
        PAD1, PAD2, PAD3, PAD4,
        PAD5, PAD6, PAD7, PAD8,
        PAD9, PAD10, PAD11, PAD12,
        PAD13, PAD14, PAD15, PAD16,

        // Buttons
        CURSOR_LEFT,
        CURSOR_RIGHT,
        CURSOR_UP,
        CURSOR_DOWN,
        REC,
        OVERDUB,
        STOP,
        PLAY,
        PLAY_START,
        MAIN_SCREEN,
        PREV_STEP_EVENT,
        NEXT_STEP_EVENT,
        GO_TO,
        PREV_BAR_START,
        NEXT_BAR_END,
        TAP,
        NEXT_SEQ,
        TRACK_MUTE,
        OPEN_WINDOW,
        FULL_LEVEL,
        SIXTEEN_LEVELS,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        SHIFT,
        ENTER,
        UNDO_SEQ,
        ERASE,
        AFTER,
        BANK_A,
        BANK_B,
        BANK_C,
        BANK_D,
        NUM_0,
        NUM_1,
        NUM_2,
        NUM_3,
        NUM_4,
        NUM_5,
        NUM_6,
        NUM_7,
        NUM_8,
        NUM_9,

        // Misc controls
        DATA_WHEEL,
        SLIDER
    };

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
