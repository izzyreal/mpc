#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc;
using namespace mpc::client::event;

ClientHardwareEvent createPadPressEvent(const PhysicalPadIndex idx)
{
    ClientHardwareEvent e;
    e.source = ClientHardwareEvent::Source::HostInputGesture;
    e.type = ClientHardwareEvent::Type::PadPress;
    e.componentId =
        static_cast<hardware::ComponentId>(hardware::PAD_1_OR_AB + idx);
    e.index = idx;
    e.value = 1.f;
    return e;
}

ClientHardwareEvent createPadReleaseEvent(const PhysicalPadIndex idx)
{
    ClientHardwareEvent e;
    e.source = ClientHardwareEvent::Source::HostInputGesture;
    e.type = ClientHardwareEvent::Type::PadRelease;
    e.componentId =
        static_cast<hardware::ComponentId>(hardware::PAD_1_OR_AB + idx);
    e.index = idx;
    e.value = 0.f;
    return e;
}

TEST_CASE("Sixteen levels", "[16-levels]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto manager = mpc.getPerformanceManager().lock();

    manager->drainQueue();

    const auto controller = mpc.clientEventController;
    const auto hwController = controller->clientHardwareEventController;

    const auto sixteenLevelsScreen =
        mpc.screens->get<lcdgui::ScreenId::Assign16LevelsScreen>();

    auto program = mpc.getSampler()->getProgram(0);

    auto note = program->getNoteFromPad(MinProgramPadIndex);

    sixteenLevelsScreen->setNote(note);

    sixteenLevelsScreen->setType(0); // Velocity

    controller->setSixteenLevelsEnabled(true);

    for (int physicalPadIndex = 0;
         physicalPadIndex < Mpc2000XlSpecs::HARDWARE_PAD_COUNT;
         ++physicalPadIndex)
    {
        hwController->handleClientHardwareEvent(
            createPadPressEvent(PhysicalPadIndex(physicalPadIndex)));

        manager->drainQueue();
        manager->drainQueue();

        auto state = manager->getSnapshot();

        auto physicalPadPress =
            state.findPhysicalPadPress(PhysicalPadIndex(physicalPadIndex));

        REQUIRE(physicalPadPress.has_value());

        auto programPadPress = state.findProgramPadPress(
            performance::PerformanceEventSource::VirtualMpcHardware,
            MinProgramPadIndex, MinProgramIndex);

        REQUIRE(programPadPress.has_value());

        REQUIRE(programPadPress->velocity == MaxVelocity);

        auto noteEvent = state.findNoteOnEvent(
            performance::PerformanceEventSource::VirtualMpcHardware, note,
            NoMidiChannel);

        REQUIRE(noteEvent.has_value());

        REQUIRE(noteEvent->velocity == MaxVelocity);

        hwController->handleClientHardwareEvent(
            createPadReleaseEvent(PhysicalPadIndex(physicalPadIndex)));

        manager->drainQueue();
        manager->drainQueue();

        physicalPadPress =
            state.findPhysicalPadPress(PhysicalPadIndex(physicalPadIndex));

        REQUIRE(!physicalPadPress.has_value());

        programPadPress = state.findProgramPadPress(
            performance::PerformanceEventSource::VirtualMpcHardware,
            ProgramPadIndex(physicalPadIndex), MinProgramIndex);

        REQUIRE(!programPadPress.has_value());

        noteEvent = state.findNoteOnEvent(
            performance::PerformanceEventSource::VirtualMpcHardware, note,
            NoMidiChannel);

        REQUIRE(!noteEvent.has_value());
    }
}
