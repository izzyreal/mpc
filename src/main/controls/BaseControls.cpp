#include "BaseControls.hpp"
#include "hardware2/Hardware2.h"

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>

#include <sampler/Pad.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

#include <lcdgui/Layer.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <lcdgui/screens/window/EditMultipleScreen.hpp>
#include <lcdgui/screens/window/ChannelSettingsScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screengroups;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sequencer;

void BaseControls::left(mpc::Mpc &mpc)
{
    if (!mpc.getLayeredScreen()->getFocusedField() || getFocusedFieldName(mpc) == "dummy")
    {
        return;
    }

    mpc.getLayeredScreen()->transferLeft();
}

void BaseControls::right(mpc::Mpc &mpc)
{
    if (!mpc.getLayeredScreen()->getFocusedField() || getFocusedFieldName(mpc) == "dummy")
    {
        return;
    }
    
    mpc.getLayeredScreen()->transferRight();
}

void BaseControls::up(mpc::Mpc &mpc)
{
    if (!mpc.getLayeredScreen()->getFocusedField() || getFocusedFieldName(mpc) == "dummy")
    {
        return;
    }
    
    mpc.getLayeredScreen()->transferUp();
}

void BaseControls::down(mpc::Mpc &mpc)
{
    if (!mpc.getLayeredScreen()->getFocusedField() || getFocusedFieldName(mpc) == "dummy")
    {
        return;
    }
    
    mpc.getLayeredScreen()->transferDown();
}

void BaseControls::function(mpc::Mpc &mpc, int i)
{
    if (i != 3)
    {
        return;
    }
    
    auto controls = mpc.getControls();
    controls->setF4Pressed(true);

    auto currentScreenName = getCurrentScreenName(mpc);

    if (mpc.getLayeredScreen()->getFocusedLayerIndex() == 1)
    {
        if (currentScreenName == "sequence")
        {
            mpc.getLayeredScreen()->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "midi-input")
        {
            mpc.getLayeredScreen()->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "midi-output")
        {
            mpc.getLayeredScreen()->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "edit-sound")
        {
            const auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
            mpc.getLayeredScreen()->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
        }
        else if (currentScreenName == "sound")
        {
            mpc.getLayeredScreen()->setPreviousScreenName(mpc.getSampler()->getPreviousScreenName());
        }
        else if (currentScreenName == "program")
        {
            mpc.getLayeredScreen()->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
            mpc.setPreviousSamplerScreenName("");
        }
        else if (currentScreenName == "name")
        {
            auto nameScreen = mpc.screens->get<NameScreen>("name");
            nameScreen->setEditing(false);
            mpc.getLayeredScreen()->setLastFocus("name", "0");
        }
        else if (currentScreenName == "directory")
        {
            const auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
            mpc.getLayeredScreen()->setPreviousScreenName(directoryScreen->previousScreenName);
        }
    }
    
    if (mpc.getLayeredScreen()->getFocusedLayerIndex() == 1 ||
        mpc.getLayeredScreen()->getFocusedLayerIndex() == 2 ||
        mpc.getLayeredScreen()->getFocusedLayerIndex() == 3)
    {
        mpc.getLayeredScreen()->openScreen(mpc.getLayeredScreen()->getPreviousScreenName());
    }
}

void BaseControls::padPressScreenUpdate(PadPressScreenUpdateContext&ctx, const int note, const std::optional<int> padIndexWithBank)
{
    if (ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    const auto screenComponent = ctx.screenComponent;

    if (isDrumNote(note) && ctx.isAllowCentralNoteAndPadUpdateScreen)
    {
        ctx.setMpcNote(note);

        if (padIndexWithBank)
        {
            ctx.setMpcPad(*padIndexWithBank);
        }
    }
    else if (auto withNotes = std::dynamic_pointer_cast<WithTimesAndNotes>(screenComponent);
             withNotes && note >= 35)
    {
        withNotes->setNote0(note);
    }
    else if (auto assign16LevelsScreen = std::dynamic_pointer_cast<Assign16LevelsScreen>(screenComponent);
             assign16LevelsScreen)
    {
        assign16LevelsScreen->setNote(note);
    }
    else if (auto editMultipleScreen = std::dynamic_pointer_cast<EditMultipleScreen>(screenComponent);
             editMultipleScreen)
    {
        editMultipleScreen->setChangeNoteTo(note);
    }
    else if (auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(screenComponent);
             stepEditorScreen && ctx.currentFieldName == "fromnote" && note > 34)
    {
        stepEditorScreen->setFromNote(note);
    }
    else if (auto mixerScreen = std::dynamic_pointer_cast<MixerScreen>(screenComponent);
             mixerScreen && padIndexWithBank)
    {
        unsigned char bankStartPadIndex = ctx.bank * 16;
        unsigned char bankEndPadIndex = bankStartPadIndex + 16;

        if (*padIndexWithBank >= bankStartPadIndex && *padIndexWithBank < bankEndPadIndex)
        {
            mixerScreen->pressPadIndexWithoutBank(*padIndexWithBank % 16);
        }
    }
    else if (auto channelSettingsScreen = std::dynamic_pointer_cast<ChannelSettingsScreen>(screenComponent);
             channelSettingsScreen)
    {
        channelSettingsScreen->setNote(note);
    }
}

void BaseControls::pad(PadPushContext &ctx, const int padIndexWithBank, int velo)
{
    if (ctx.isSoundScreen)
    {
        ctx.basicPlayer.mpcNoteOn(ctx.soundIndex, 127, 0);
        return;
    }
    
    const auto padWasNotPressed = !ctx.controls->isPadPressed(padIndexWithBank);

    ctx.controls->pressPad(padIndexWithBank);

    if (ctx.currentScreenName == "sequencer" &&
        (ctx.controls->isTapPressed() || ctx.controls->isNoteRepeatLocked()) &&
        ctx.sequencer->isPlaying() &&
        padWasNotPressed)
    {
        return;
    }

    if (ctx.isFullLevelEnabled)
    {
        velo = 127;
    }

    if (ctx.isRecordingOrOverdubbing && ctx.isErasePressed)
    {
        return;
    }
    
    if (ctx.isNoteRepeatLocked)
    {
        return;
    }

    const auto note = ctx.track->getBus() > 0 ? ctx.program->getPad(padIndexWithBank)->getNote() : padIndexWithBank + 35;

    PadPressScreenUpdateContext padPressScreenUpdateContext {
        ctx.currentScreenName,
        ctx.isSixteenLevelsEnabled,
        mpc::sequencer::isDrumNote(note),
        ctx.allowCentralNoteAndPadUpdate,
        ctx.screenComponent, ctx.setMpcNote,
        ctx.setMpcPad,
        ctx.currentFieldName,
        ctx.bank
    };

    padPressScreenUpdate(padPressScreenUpdateContext, note, padIndexWithBank);

    GenerateNoteOnContext generateNoteOnContext {
        ctx.isSixteenLevelsEnabled,
        ctx.isRecordingOrOverdubbing,
        ctx.isStepRecording,
        ctx.isRecMainWithoutPlaying,
        ctx.soundIndex,
        ctx.trackBus,
        ctx.currentBarIndex,
        ctx.tickPosition,
        ctx.noteValueLengthInTicks,
        ctx.swing,
        ctx.metronomeOnlyTickPosition,
        ctx.hardwareSliderValue,
        ctx.drumScreenSelectedDrum,
        ctx.currentScreenIsSamplerScreen,
        ctx.program,
        ctx.track,
        ctx.sequencer,
        ctx.controls,
        ctx.audioMidiServices,
        ctx.timingCorrectScreen,
        ctx.assign16LevelsScreen,
        ctx.eventHandler,
        ctx.frameSequencer };
    generateNoteOn(generateNoteOnContext, note, velo, padIndexWithBank);
}

void BaseControls::generateNoteOn(const GenerateNoteOnContext& ctx, const int note, const int velo, const int padIndexWithBank)
{
    const bool is16LevelsEnabled = ctx.isSixteenLevelsEnabled;

    const auto playOnEvent = std::make_shared<NoteOnEventPlayOnly>(note, velo);

    const auto assign16LevelsScreen = ctx.assign16LevelsScreen;

    Util::SixteenLevelsContext sixteenLevelsContext {
        is16LevelsEnabled,
        assign16LevelsScreen->getType(),
        assign16LevelsScreen->getOriginalKeyPad(),
        assign16LevelsScreen->getNote(),
        assign16LevelsScreen->getParameter(),
        padIndexWithBank % 16
    };

    Util::set16LevelsValues(sixteenLevelsContext, playOnEvent);

    const bool isSliderNote = ctx.program && ctx.program->getSlider()->getNote() == note;
    auto programSlider = ctx.program->getSlider();

    Util::SliderNoteVariationContext sliderNoteVariationContext {
        ctx.hardwareSliderValue,
        programSlider->getNote(),
        programSlider->getParameter(),
        programSlider->getTuneLowRange(),
        programSlider->getTuneHighRange(),
        programSlider->getDecayLowRange(),
        programSlider->getDecayHighRange(),
        programSlider->getAttackLowRange(),
        programSlider->getAttackHighRange(),
        programSlider->getFilterLowRange(),
        programSlider->getFilterHighRange()
    };

    if (ctx.program && isSliderNote)
    {
        Util::setSliderNoteVariationParameters(sliderNoteVariationContext, playOnEvent);
    }

    const auto drum = ctx.currentScreenIsSamplerScreen ? 
            std::optional<uint8_t>(ctx.drumScreenSelectedDrum) : std::optional<uint8_t>();

    ctx.controls->storePlayNoteEvent(padIndexWithBank, playOnEvent);
    ctx.eventHandler->handle(playOnEvent, ctx.track.get(), drum);

    std::shared_ptr<NoteOnEvent> recordNoteOnEvent;

    if (ctx.sequencer->isRecordingOrOverdubbing())
    {
        recordNoteOnEvent = ctx.track->recordNoteEventASync(note, velo);
    }
    else if (ctx.controls->isStepRecording() && (ctx.track->getBus() == 0 || isDrumNote(note)))
    {
        recordNoteOnEvent = ctx.track->recordNoteEventSynced(ctx.sequencer->getTickPosition(), note, velo);
        ctx.sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(ctx.frameSequencer->getMetronomeOnlyTickPosition());
    }
    else if (ctx.controls->isRecMainWithoutPlaying())
    {
        recordNoteOnEvent = ctx.track->recordNoteEventSynced(ctx.sequencer->getTickPosition(), note, velo);
        ctx.sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(ctx.frameSequencer->getMetronomeOnlyTickPosition());

        const auto timingCorrectScreen = ctx.timingCorrectScreen;
        const int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

        if (stepLength != 1)
        {
            const int bar = ctx.sequencer->getCurrentBarIndex() + 1;
            const auto correctedTick = ctx.track->timingCorrectTick(0,
                                                                bar,
                                                                ctx.sequencer->getTickPosition(),
                                                                stepLength,
                                                                timingCorrectScreen->getSwing());

            if (ctx.sequencer->getTickPosition() != correctedTick)
            {
                ctx.sequencer->move(Sequencer::ticksToQuarterNotes(correctedTick));
            }
        }
    }

    if (recordNoteOnEvent)
    {
        if (is16LevelsEnabled)
        {
            Util::set16LevelsValues(sixteenLevelsContext, recordNoteOnEvent);
        }

        if (ctx.program && isSliderNote)
        {
            Util::setSliderNoteVariationParameters(sliderNoteVariationContext, recordNoteOnEvent);
        }

        ctx.controls->storeRecordNoteEvent(padIndexWithBank, recordNoteOnEvent);
    }
}

void BaseControls::numpad(mpc::Mpc &mpc, int i)
{
    auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        const auto fieldName = getFocusedFieldName(mpc);

        if (isTypableField(getCurrentScreenName(mpc), fieldName))
        {
            auto field = mpc.getLayeredScreen()->getFocusedField();
        
            if (!field->isTypeModeEnabled())
            {
                field->enableTypeMode();
            }
            
            field->type(i);
        }
    }
    
    if (controls->isShiftPressed())
    {
        switch (i)
        {
            case 0:
                mpc.getLayeredScreen()->openScreen("vmpc-settings");
                break;
            case 1:
                if (mpc.getSequencer()->isPlaying())
                {
                    return;
                }
                
                mpc.getLayeredScreen()->openScreen("song");
                break;
            case 2:
                mpc.getLayeredScreen()->openScreen("punch");
                break;
            case 3:
            {
                if (mpc.getSequencer()->isPlaying())
                {
                    break;
                }
                
                mpc.getLayeredScreen()->openScreen("load");
                break;
            }
            case 4:
                if (mpc.getSequencer()->isPlaying())
                {
                    break;
                }
                
                mpc.getLayeredScreen()->openScreen("sample");
                break;
            case 5:
                if (mpc.getSequencer()->isPlaying())
                {
                    break;
                }
                
                mpc.getLayeredScreen()->openScreen("trim");
                break;
            case 6:
            {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;

                if (newDrum >= 0)
                {
                    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                    drumScreen->setDrum(newDrum);
                }

                mpc.getLayeredScreen()->openScreen("select-drum");
                break;
            }
            case 7:
            {
                auto newDrum = mpc.getSequencer()->getActiveTrack()->getBus() - 1;

                if (newDrum >= 0)
                {
                    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                    drumScreen->setDrum(newDrum);
                }

                mpc.getLayeredScreen()->openScreen("select-mixer-drum");
                break;
            }
            case 8:
                if (mpc.getSequencer()->isPlaying())
                {
                    break;
                }
                
                mpc.getLayeredScreen()->openScreen("others");
                break;
            case 9:
                if (mpc.getSequencer()->isPlaying())
                {
                    break;
                }
                
                mpc.getLayeredScreen()->openScreen("sync");
                break;
        }
        
    }
}

void BaseControls::pressEnter(mpc::Mpc &mpc)
{
    auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        mpc.getLayeredScreen()->openScreen("save");
    }
}

void BaseControls::rec(mpc::Mpc &mpc)
{
    if (isPlayOnlyScreen(getCurrentScreenName(mpc)))
    {
        return;
    }

    auto controls = mpc.getControls();
    
    if (controls->isRecPressed(/*includeLocked=*/false))
    {
        return;
    }

    controls->setRecPressed(true);

    controls->setRecLocked(false);

    if (mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->setRecording(false);
        mpc.getSequencer()->setOverdubbing(false);
    }

    if (!isPlayAndRecordScreen(getCurrentScreenName(mpc)))
    {
        mpc.getLayeredScreen()->openScreen("sequencer");
    }

    mpc.getHardware2()->getLed("rec")->setEnabled(true);
}

void BaseControls::overDub(mpc::Mpc &mpc)
{
    if (isPlayOnlyScreen(getCurrentScreenName(mpc)))
    {
        return;
    }

    auto controls = mpc.getControls();

    if (controls->isOverDubPressed(/*includeLocked=*/false))
    {
        return;
    }

    controls->setOverDubPressed(true);

    controls->setOverDubLocked(false);

    if (mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->setRecording(false);
        mpc.getSequencer()->setOverdubbing(false);
    }

    if (!isPlayAndRecordScreen(getCurrentScreenName(mpc)))
    {
        mpc.getLayeredScreen()->openScreen("sequencer");
    }

    mpc.getHardware2()->getLed("overdub")->setEnabled(true);
}

void BaseControls::stop(mpc::Mpc &mpc)
{
    const auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
    const auto ams = mpc.getAudioMidiServices();
    const auto controls = mpc.getControls();
    
    if (controls->isNoteRepeatLocked())
    {
        controls->setNoteRepeatLocked(false);
    }
    
    if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->record != 4 || controls->isShiftPressed()))
        ams->stopBouncingEarly();
    
    mpc.getSequencer()->stop();
    
    if (!isPlayScreen(getCurrentScreenName(mpc)))
    {
        mpc.getLayeredScreen()->openScreen("sequencer");
    }

    mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverDubbing());
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
    mpc.getHardware2()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
}

void BaseControls::play(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();

    if (controls->isPlayPressed())
    {
        return;
    }

    controls->setPlayPressed(true);

    if (mpc.getSequencer()->isPlaying())
    {
        if (controls->isRecPressed() && !mpc.getSequencer()->isOverDubbing())
        {
            mpc.getSequencer()->setOverdubbing(false);
            mpc.getSequencer()->setRecording(true);
        }
        else if (controls->isOverDubPressed() && !mpc.getSequencer()->isRecording())
        {
            mpc.getSequencer()->setOverdubbing(true);
            mpc.getSequencer()->setRecording(false);
        }
    }
    else
    {
        const auto currentScreenName = getCurrentScreenName(mpc);
        const bool currentScreenAllowsPlayAndRecord = isPlayAndRecordScreen(currentScreenName);

        if (controls->isRecPressed())
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }
            
            mpc.getSequencer()->rec();
        }
        else if (controls->isOverDubPressed())
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }
            
            mpc.getSequencer()->overdub();
        }
        else {
            if (controls->isShiftPressed() && !mpc.getAudioMidiServices()->isBouncing())
            {
                mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!isPlayScreen(currentScreenName))
                {
                    mpc.getLayeredScreen()->openScreen("sequencer");
                }
                
                mpc.getSequencer()->setSongModeEnabled(currentScreenName == "song");
                mpc.getSequencer()->play();
            }
        }
    }

    mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverDubbing());
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
    mpc.getHardware2()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
}

void BaseControls::playStart(mpc::Mpc &mpc)
{
    if (mpc.getSequencer()->isPlaying())
    {
        return;
    }

    const auto controls = mpc.getControls();

    const auto currentScreenName = getCurrentScreenName(mpc);

    const bool currentScreenAllowsPlayAndRecord = isPlayAndRecordScreen(currentScreenName);
    
    if (controls->isRecPressed())
    {
        if (!currentScreenAllowsPlayAndRecord)
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }
        
        mpc.getSequencer()->recFromStart();
    }
    else if (controls->isOverDubPressed())
    {
        if (!currentScreenAllowsPlayAndRecord)
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }
        
        mpc.getSequencer()->overdubFromStart();
    }
    else
    {
        if (controls->isShiftPressed())
        {
            mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");
        }
        else
        {
            if (!isPlayScreen(getCurrentScreenName(mpc)))
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }
            
            mpc.getSequencer()->setSongModeEnabled(currentScreenName == "song");
            mpc.getSequencer()->playFromStart();
        }
    }

    mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverDubbing());
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
    mpc.getHardware2()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
}

void BaseControls::mainScreen(mpc::Mpc &mpc)
{
    const auto ams = mpc.getAudioMidiServices();
    
    if (ams->isRecordingSound())
    {
        ams->stopSoundRecorder();
    }

    mpc.getLayeredScreen()->openScreen("sequencer");
    mpc.getSequencer()->setSoloEnabled(mpc.getSequencer()->isSoloEnabled());
    
    const auto hw = mpc.getHardware2();
    hw->getLed("next-seq")->setEnabled(false);
    hw->getLed("track-mute")->setEnabled(false);
}

void BaseControls::tap(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();

    if (controls->isTapPressed())
    {
        return;
    }

    controls->setTapPressed(true);
    mpc.getSequencer()->tap();
}

void BaseControls::goTo(mpc::Mpc &mpc)
{
    if (!mpc.getSequencer()->getActiveSequence()->isUsed())
    {
        return;
    }

    const auto controls = mpc.getControls();
    controls->setGoToPressed(true);

    if (getCurrentScreenName(mpc) == "sequencer")
    {
        mpc.getLayeredScreen()->openScreen("locate");
    }
}

void BaseControls::nextSeq(mpc::Mpc &mpc)
{
    const auto currentScreenName = getCurrentScreenName(mpc);

    if (currentScreenName == "next-seq" ||
        currentScreenName == "next-seq-pad")
    {
        mpc.getLayeredScreen()->openScreen("sequencer");
        mpc.getHardware2()->getLed("next-seq")->setEnabled(false);
    }
    else if (currentScreenName == "sequencer" ||
             currentScreenName == "track-mute")
    {
        Util::initSequence(mpc);
        mpc.getLayeredScreen()->openScreen("next-seq");
        mpc.getHardware2()->getLed("next-seq")->setEnabled(true);
        mpc.getHardware2()->getLed("track-mute")->setEnabled(false);
    }
}


void BaseControls::trackMute(mpc::Mpc &mpc)
{
    const auto currentScreenName = getCurrentScreenName(mpc);

    if (currentScreenName == "track-mute")
    {
        auto previous = getPreviousScreenName(mpc);

        if (previous == "next-seq" || previous == "next-seq-pad")
        {
            mpc.getLayeredScreen()->openScreen("next-seq");
        }
        else
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }
        
        mpc.getHardware2()->getLed("track-mute")->setEnabled(false);
    }
    else if
        (currentScreenName == "next-seq" ||
         currentScreenName == "next-seq-pad" ||
         currentScreenName == "sequencer")
    {
        Util::initSequence(mpc);
        mpc.getLayeredScreen()->openScreen("track-mute");
        mpc.getHardware2()->getLed("track-mute")->setEnabled(true);
    }
}

void BaseControls::bank(mpc::Mpc &mpc, int i)
{
    mpc.setBank(i);
}

void BaseControls::fullLevel(mpc::Mpc &mpc)
{
    mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
    mpc.getHardware2()->getLed("full-level")->setEnabled(mpc.isFullLevelEnabled());
}

void BaseControls::sixteenLevels(mpc::Mpc &mpc)
{
    const auto currentScreenName = getCurrentScreenName(mpc);

    if (currentScreenName != "sequencer" &&
        currentScreenName != "assign-16-levels")
    {
        return;
    }
    
    if (mpc.isSixteenLevelsEnabled())
    {
        mpc.setSixteenLevelsEnabled(false);
        mpc.getHardware2()->getLed("sixteen-levels")->setEnabled(false);
    }
    else
    {
        mpc.getLayeredScreen()->openScreen("assign-16-levels");
    }
}

void BaseControls::after(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        mpc.getLayeredScreen()->openScreen("assign");
    }
    else
    {
        mpc.setAfterEnabled(!mpc.isAfterEnabled());
        mpc.getHardware2()->getLed("after")->setEnabled(mpc.isAfterEnabled());
    }
}

void BaseControls::shift(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        return;
    }
    
    controls->setShiftPressed(true);
    
    auto field = mpc.getLayeredScreen()->getFocusedField();

    if (!field || !field->isTypeModeEnabled())
    {
        return;
    }

    field->disableTypeMode();

    const auto split = field->getActiveSplit();
    
    if (split != -1)
    {
        field->setSplit(true);
        field->setActiveSplit(split);
    }
}

void BaseControls::undoSeq(mpc::Mpc &mpc)
{
    mpc.getSequencer()->undoSeq();
}

void BaseControls::erase(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    controls->setErasePressed(true);
    
    if (!mpc.getSequencer()->getActiveSequence()->isUsed())
    {
        return;
    }
    
    if (!mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getLayeredScreen()->openScreen("erase");
    }
}

int BaseControls::getSoundIncrement(const int64_t frameCount, int notch_inc)
{
    auto soundInc = notch_inc;
    
    if (abs(notch_inc) != 1)
    {
        soundInc *= (int)(ceil(frameCount / 15000.0));
    }
    
    return soundInc;
}

void BaseControls::splitLeft(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        BaseControls::left(mpc);
        return;
    }
    
    if (!isFieldSplittable(getCurrentScreenName(mpc), getFocusedFieldName(mpc)))
    {
        return;
    }

    const auto field = mpc.getLayeredScreen()->getFocusedField();
    
    if (field->isSplit())
    {
        field->setActiveSplit(field->getActiveSplit() - 1);
    }
    else
    {
        field->setSplit(true);
    }
}

void BaseControls::splitRight(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        BaseControls::right(mpc);
        return;
    }
    
    const auto field = mpc.getLayeredScreen()->getFocusedField();

    if (isFieldSplittable(getCurrentScreenName(mpc), getFocusedFieldName(mpc)) && field->isSplit())
    {
        if (!field->setActiveSplit(field->getActiveSplit() + 1))
        {
            field->setSplit(false);
        }
    }
}

