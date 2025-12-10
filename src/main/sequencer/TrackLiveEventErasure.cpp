#include "sequencer/Track.hpp"

#include "sequencer/TrackStateView.hpp"
#include "sequencer/Bus.hpp"

#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

#include "sampler/Sampler.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

bool Track::shouldRemovePlayIndexEventDueToRecording() const
{
    if (!isRecording() || getIndex() == TempoChangeTrackIndex)
    {
        return false;
    }

    if (!isPunchEnabled())
    {
        if (isRecordingModeMulti() || trackIndex == getActiveTrackIndex())
        {
            return true;
        }

        return false;
    }

    const auto autoPunchMode = getAutoPunchMode();
    const auto pos = getTickPosition();
    const auto punchInTime = getPunchInTime();

    if (autoPunchMode == 0 && pos >= punchInTime)
    {
        return true;
    }

    const auto punchOutTime = getPunchOutTime();

    if (autoPunchMode == 1 && pos < punchOutTime)
    {
        return true;
    }

    if (autoPunchMode == 2 && pos >= punchInTime && pos < punchOutTime)
    {
        return true;
    }

    return false;
}

bool Track::shouldRemovePlayIndexEventDueToErasePressed() const
{
    if (!isOverdubbing() || getIndex() == TempoChangeTrackIndex ||
        !isEraseButtonPressed())
    {
        return false;
    }

    if (!isRecordingModeMulti() && trackIndex != getActiveTrackIndex())
    {
        return false;
    }

    const auto snapshot = getSnapshot(getIndex());
    const auto playEventIndex = snapshot.getPlayEventIndex();
    const auto event = snapshot.getEventByIndex(playEventIndex);

    if (!event || event->type != EventType::NoteOn)
    {
        return false;
    }

    const auto busType = getBusType();

    const auto drumBus = std::dynamic_pointer_cast<DrumBus>(
        getSequencerBus(busType == BusType::MIDI ? BusType::DRUM1 : busType));

    const auto programIndex = drumBus->getProgramIndex();
    const auto program = sampler->getProgram(programIndex);

    const auto noteNumber = event->noteNumber;

    bool oneOrMorePadsArePressed = false;

    for (int8_t programPadIndex = 0;
         programPadIndex < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; ++programPadIndex)
    {
        if (isProgramPadPressed(ProgramPadIndex(programPadIndex), programIndex))
        {
            if (program->getNoteFromPad(ProgramPadIndex(programPadIndex)) ==
                noteNumber)
            {
                return true;
            }

            oneOrMorePadsArePressed = true;
            break;
        }
    }

    if (!oneOrMorePadsArePressed || !isSixteenLevelsEnabled())
    {
        return false;
    }

    const auto vmpcSettingsScreen =
        getScreens()->get<ScreenId::VmpcSettingsScreen>();

    if (vmpcSettingsScreen->sixteenLevelsEraseMode == 0)
    {
        return true;
    }

    const auto assign16LevelsScreen =
        getScreens()->get<ScreenId::Assign16LevelsScreen>();

    const auto sixteenLevelsOriginalKeyPadIndex =
        assign16LevelsScreen->getOriginalKeyPad();

    const auto sixteenLevelsType = assign16LevelsScreen->getType();

    for (int8_t programPadIndex = 0;
         programPadIndex < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; ++programPadIndex)
    {
        if (!isProgramPadPressed(ProgramPadIndex(programPadIndex),
                                 programIndex))
        {
            continue;
        }

        const auto physicalPadIndex = PhysicalPadIndex(programPadIndex % 16);

        if (sixteenLevelsType == 0)
        {
            const auto diff =
                physicalPadIndex - sixteenLevelsOriginalKeyPadIndex;
            const auto candidate = std::clamp(64 + diff * 5, 4, 124);

            if (candidate == event->noteVariationValue)
            {
                return true;
            }

            continue;
        }

        const auto candidate =
            static_cast<int>(floor(100.f / Mpc2000XlSpecs::HARDWARE_PAD_COUNT) *
                             physicalPadIndex);

        if (candidate == event->noteVariationValue)
        {
            return true;
        }
    }

    return false;
}