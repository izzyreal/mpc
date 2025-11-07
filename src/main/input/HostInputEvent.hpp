#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include "hardware/ComponentId.hpp"

#include <variant>
#include <stdexcept>

namespace mpc::input
{

    struct FocusEvent
    {
        enum class Type
        {
            Lost,
            Gained
        };
        Type type;
    };

    struct KeyEvent
    {
        bool keyDown;
        int rawKeyCode;
        bool shiftDown;
        bool ctrlDown;
        bool altDown;
    };

    struct GestureEvent
    {
        enum class Type
        {
            BEGIN,  // Initial contact: mouse button pressed or finger touched.
            UPDATE, // Continued motion of that same contact (mouse drag or
                    // finger move), or scroll motions without BEGIN/END.
            END, // Termination of the same contact: corresponding mouse button
                 // released or finger lifted.
            REPEAT // Rapid successive BEGIN, END, BEGIN, END on the same
                   // component, i.e. double/triple click/tap.
        };

        enum class Movement
        {
            None,     // No movement data (e.g. BEGIN/END/REPEAT events).
            Absolute, // Absolute motion in normalized coordinates (e.g. mouse
                      // drag along a slider).
            Relative  // Relative motion in continuous deltas (e.g. mouse wheel
                      // or encoder turn).
        };

        Type type;

        // Specifies whether this event represents absolute or relative motion.
        Movement movement = Movement::None;

        // Normalized pointer position in [0, 1] relative to the component's
        // bounds
        float normY = 0.0f;

        // Only valid for UPDATE events with movement == Relative
        float continuousDelta = 0.f;

        // Only valid for REPEAT events. 2 for double, 3 for triple click/tap.
        int repeatCount = 0;
        
        int sourceIndex = 0;

        hardware::ComponentId componentId;
    };

    /* Input events coming from the host, i.e. the machine the virtual MPC2000XL
     * is virtualized on */
    struct HostInputEvent
    {
        enum class Source
        {
            KEYBOARD,
            GESTURE,
            MIDI,
            FOCUS
        };

        explicit HostInputEvent(KeyEvent k) : payload(std::move(k)) {}
        explicit HostInputEvent(GestureEvent g) : payload(std::move(g)) {}
        explicit HostInputEvent(client::event::ClientMidiEvent m)
            : payload(std::move(m))
        {
        }
        explicit HostInputEvent(FocusEvent f) : payload(std::move(f)) {}

        Source getSource() const
        {
            if (std::holds_alternative<KeyEvent>(payload))
            {
                return Source::KEYBOARD;
            }
            if (std::holds_alternative<GestureEvent>(payload))
            {
                return Source::GESTURE;
            }
            if (std::holds_alternative<client::event::ClientMidiEvent>(payload))
            {
                return Source::MIDI;
            }
            if (std::holds_alternative<FocusEvent>(payload))
            {
                return Source::FOCUS;
            }

            // Defensive: someone added a new variant type but forgot to update
            // getSource()
            throw std::logic_error(
                "HostInputEvent::getSource() encountered unknown payload type");
        }

        std::variant<KeyEvent, GestureEvent, client::event::ClientMidiEvent,
                     FocusEvent>
            payload;
    };

} // namespace mpc::input
