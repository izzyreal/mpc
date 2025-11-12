#pragma once

#include "eventregistry/State.hpp"
#include "concurrency/AtomicStateExchange.hpp"
#include "IntTypes.hpp"
#include "eventregistry/Source.hpp"
#include "eventregistry/EventTypes.hpp"
#include "eventregistry/EventMessage.hpp"
#include "eventregistry/StateView.hpp"
#include "sequencer/BusType.hpp"

#include <optional>
#include <functional>

namespace mpc::eventregistry
{
    class EventRegistry
        : public concurrency::AtomicStateExchange<State, StateView,
                                                  EventMessage>
    {
    public:
        EventRegistry();

        void registerPhysicalPadPress(
            Source, lcdgui::ScreenId, sequencer::BusType, PhysicalPadIndex,
            Velocity, TrackIndex, controller::Bank, std::optional<ProgramIndex>,
            std::optional<NoteNumber>,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadAftertouch(
            PhysicalPadIndex, Pressure, Source,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadRelease(
            PhysicalPadIndex, Source,
            const std::function<void(void *)> &action) const;

        void registerProgramPadPress(
            Source, std::optional<MidiChannel> midiInputChannel,
            lcdgui::ScreenId, TrackIndex, sequencer::BusType, ProgramPadIndex,
            Velocity, ProgramIndex) const;

        void registerProgramPadAftertouch(Source, ProgramPadIndex, ProgramIndex,
                                          Pressure) const;

        void registerProgramPadRelease(
            Source, ProgramPadIndex, ProgramIndex,
            const std::function<void(void *)> &action) const;

        NoteOnEvent
        registerNoteOn(Source, std::optional<MidiChannel> midiInputChannel,
                       lcdgui::ScreenId, TrackIndex, sequencer::BusType,
                       NoteNumber, Velocity, std::optional<ProgramIndex>,
                       const std::function<void(void *)> &action) const;

        void registerNoteAftertouch(
            Source, NoteNumber, Pressure,
            std::optional<MidiChannel> midiInputChannel) const;

        void registerNoteOff(Source, NoteNumber,
                             std::optional<MidiChannel> midiInputChannel,
                             const std::function<void(void *)> &action) const;

        void clear();

    protected:
        void applyMessage(const EventMessage &) noexcept override;

    private:
        static constexpr size_t CAPACITY = 8192;
        void reserveState(State &);
    };
} // namespace mpc::eventregistry
