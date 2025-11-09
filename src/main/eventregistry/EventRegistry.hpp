#pragma once

#include "eventregistry/State.hpp"

#include "concurrency/AtomicStateExchange.hpp"

#include "IntTypes.hpp"

#include "eventregistry/Source.hpp"
#include "eventregistry/EventTypes.hpp"
#include "eventregistry/EventMessage.hpp"

#include "eventregistry/StateView.hpp"

#include <memory>
#include <optional>
#include <functional>

namespace mpc::sampler
{
    class Program;
}
namespace mpc::lcdgui
{
    class ScreenComponent;
}
namespace mpc::sequencer
{
    class Track;
    class Bus;
    class NoteOnEvent;
} // namespace mpc::sequencer

namespace mpc::eventregistry
{
    class EventRegistry
        : public mpc::concurrency::AtomicStateExchange<State, StateView,
                                                       EventMessage>
    {
    public:
        EventRegistry();

        void registerPhysicalPadPress(
            Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
            const std::shared_ptr<sequencer::Bus> &, PhysicalPadIndex padIndex,
            Velocity, sequencer::Track *, int bank, std::optional<int> note,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadAftertouch(
            PhysicalPadIndex, Pressure, Source source,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadRelease(
            PhysicalPadIndex, Source source,
            const std::function<void(void *)> &action) const;

        ProgramPadPressEventPtr registerProgramPadPress(
            Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
            const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            Velocity, sequencer::Track *, std::optional<MidiChannel>) const;

        void registerProgramPadAftertouch(
            Source, const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            Pressure, sequencer::Track *) const;

        void registerProgramPadRelease(
            Source, const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            sequencer::Track *, std::optional<MidiChannel>,
            const std::function<void(void *)> &action) const;

        NoteOnEventPtr
        registerNoteOn(Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
                       const std::shared_ptr<sequencer::Bus> &, NoteNumber,
                       Velocity, sequencer::Track *, std::optional<MidiChannel>,
                       const std::shared_ptr<sampler::Program> &,
                       const std::function<void(void *)> &action) const;

        void registerNoteAftertouch(Source, NoteNumber, Pressure,
                                    std::optional<MidiChannel>) const;
        void registerNoteOff(Source, NoteNumber, std::optional<MidiChannel>,
                             const std::function<void(void *)> &action) const;

        void clear();

    protected:
        void reserveState(State &s) override;
        void applyMessage(const EventMessage &msg) noexcept override;

    private:
        const size_t CAPACITY = 8192;
    };
} // namespace mpc::eventregistry
