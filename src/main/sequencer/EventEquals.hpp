#pragma once
#include "sequencer/Event.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"

#include <memory>

namespace mpc::sequencer
{
    inline bool operator==(const Event &a, const Event &b)
    {
        return a.getTick() == b.getTick() && a.getTrack() == b.getTrack() &&
               a.getTypeName() == b.getTypeName();
    }

    inline bool operator==(const NoteOffEvent &a, const NoteOffEvent &b)
    {
        return a == b && a.getNote() == b.getNote();
    }

    inline bool operator==(const NoteOnEvent &a, const NoteOnEvent &b)
    {
        return a == b && a.getNote() == b.getNote() &&
               a.getVelocity() == b.getVelocity() &&
               a.getDuration().has_value() == b.getDuration().has_value() &&
               (!a.getDuration().has_value() ||
                *a.getDuration() == *b.getDuration()) &&
               a.getVariationType() == b.getVariationType() &&
               a.getVariationValue() == b.getVariationValue() &&
               ((!a.getNoteOff() && !b.getNoteOff()) ||
                (a.getNoteOff() && b.getNoteOff() &&
                 *a.getNoteOff() == *b.getNoteOff())) &&
               a.isFinalized() == b.isFinalized();
    }

    inline bool operator==(const MixerEvent &a, const MixerEvent &b)
    {
        return a == b && a.getParameter() == b.getParameter() &&
               a.getPad() == b.getPad() && a.getValue() == b.getValue();
    }

    inline bool operator==(const ChannelPressureEvent &a,
                           const ChannelPressureEvent &b)
    {
        return a == b && a.getAmount() == b.getAmount();
    }

    inline bool operator==(const ControlChangeEvent &a,
                           const ControlChangeEvent &b)
    {
        return a == b && a.getController() == b.getController() &&
               a.getAmount() == b.getAmount();
    }

    inline bool operator==(const PitchBendEvent &a, const PitchBendEvent &b)
    {
        return a == b && a.getAmount() == b.getAmount();
    }

    inline bool operator==(const PolyPressureEvent &a,
                           const PolyPressureEvent &b)
    {
        return a == b && a.getNote() == b.getNote() &&
               a.getAmount() == b.getAmount();
    }

    inline bool operator==(const ProgramChangeEvent &a,
                           const ProgramChangeEvent &b)
    {
        return a == b && a.getProgram() == b.getProgram();
    }

    inline bool operator==(const SystemExclusiveEvent &a,
                           const SystemExclusiveEvent &b)
    {
        return a == b && a.getBytes() == b.getBytes();
    }

    inline bool eventsEqual(const std::shared_ptr<Event> &a,
                            const std::shared_ptr<Event> &b)
    {
        if (!a || !b)
        {
            return !a && !b;
        }

        if (a->getTypeName() != b->getTypeName())
        {
            return false;
        }

        if (const auto na = std::dynamic_pointer_cast<NoteOnEvent>(a))
        {
            return *na == *std::dynamic_pointer_cast<NoteOnEvent>(b);
        }
        if (const auto no = std::dynamic_pointer_cast<NoteOffEvent>(a))
        {
            return *no == *std::dynamic_pointer_cast<NoteOffEvent>(b);
        }
        if (const auto me = std::dynamic_pointer_cast<MixerEvent>(a))
        {
            return *me == *std::dynamic_pointer_cast<MixerEvent>(b);
        }
        if (const auto ce = std::dynamic_pointer_cast<ChannelPressureEvent>(a))
        {
            return *ce == *std::dynamic_pointer_cast<ChannelPressureEvent>(b);
        }
        if (const auto cc = std::dynamic_pointer_cast<ControlChangeEvent>(a))
        {
            return *cc == *std::dynamic_pointer_cast<ControlChangeEvent>(b);
        }
        if (const auto pb = std::dynamic_pointer_cast<PitchBendEvent>(a))
        {
            return *pb == *std::dynamic_pointer_cast<PitchBendEvent>(b);
        }
        if (const auto pe = std::dynamic_pointer_cast<PolyPressureEvent>(a))
        {
            return *pe == *std::dynamic_pointer_cast<PolyPressureEvent>(b);
        }
        if (const auto pg = std::dynamic_pointer_cast<ProgramChangeEvent>(a))
        {
            return *pg == *std::dynamic_pointer_cast<ProgramChangeEvent>(b);
        }
        if (const auto sy = std::dynamic_pointer_cast<SystemExclusiveEvent>(a))
        {
            return *sy == *std::dynamic_pointer_cast<SystemExclusiveEvent>(b);
        }

        // fallback for unknown or base Event
        return *a == *b;
    }

    inline std::shared_ptr<Event> cloneEvent(const std::shared_ptr<Event> &e)
    {
        using namespace mpc::sequencer;
        if (!e)
        {
            return nullptr;
        }

        if (const auto n = std::dynamic_pointer_cast<NoteOnEvent>(e))
        {
            return std::make_shared<NoteOnEvent>(*n);
        }
        if (const auto n = std::dynamic_pointer_cast<NoteOffEvent>(e))
        {
            return std::make_shared<NoteOffEvent>(*n);
        }
        if (const auto m = std::dynamic_pointer_cast<MixerEvent>(e))
        {
            return std::make_shared<MixerEvent>(*m);
        }
        if (const auto c = std::dynamic_pointer_cast<ControlChangeEvent>(e))
        {
            return std::make_shared<ControlChangeEvent>(*c);
        }
        if (const auto p = std::dynamic_pointer_cast<PitchBendEvent>(e))
        {
            return std::make_shared<PitchBendEvent>(*p);
        }
        if (const auto pp = std::dynamic_pointer_cast<PolyPressureEvent>(e))
        {
            return std::make_shared<PolyPressureEvent>(*pp);
        }
        if (const auto ch = std::dynamic_pointer_cast<ChannelPressureEvent>(e))
        {
            return std::make_shared<ChannelPressureEvent>(*ch);
        }
        if (const auto pr = std::dynamic_pointer_cast<ProgramChangeEvent>(e))
        {
            return std::make_shared<ProgramChangeEvent>(*pr);
        }
        if (const auto sx = std::dynamic_pointer_cast<SystemExclusiveEvent>(e))
        {
            return std::make_shared<SystemExclusiveEvent>(*sx);
        }

        return nullptr;
    }
} // namespace mpc::sequencer
