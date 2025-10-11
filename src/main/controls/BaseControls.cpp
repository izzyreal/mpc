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
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sequencer;

BaseControls::BaseControls(mpc::Mpc& _mpc) :
    mpc (_mpc),
    ls (_mpc.getLayeredScreen()),
    sampler (_mpc.getSampler()),
    sequencer (_mpc.getSequencer())
{
}

void BaseControls::init()
{
    currentScreenName = ls->getCurrentScreenName();
    param = ls->getFocus();
    activeField = ls->getFocusedLayer()->findField(param);
    
    const auto isSampler = collectionContainsCurrentScreen(samplerScreens);
    
    if (isSampler)
    {
        splittable = param == "st" || param == "end" || param == "to" || param == "endlengthvalue" || param == "start";
    }
    else
    {
        splittable = false;
    }
}

void BaseControls::left()
{
    init();

    if (!activeField || param == "dummy")
    {
        return;
    }

    ls->transferLeft();
}

void BaseControls::right()
{
    init();

    if (!activeField || param == "dummy")
    {
        return;
    }
    
    ls->transferRight();
}

void BaseControls::up()
{
    init();
    
    if (!activeField || param == "dummy")
    {
        return;
    }
    
    ls->transferUp();
}

void BaseControls::down()
{
    init();
    
    if (!activeField || param == "dummy")
    {
        return;
    }
    
    ls->transferDown();
}

void BaseControls::function(int i)
{
    init();

    if (i != 3)
    {
        return;
    }
    
    auto controls = mpc.getControls();
    controls->setF4Pressed(true);

    if (ls->getFocusedLayerIndex() == 1)
    {
        if (currentScreenName == "sequence")
        {
            ls->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "midi-input")
        {
            ls->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "midi-output")
        {
            ls->setPreviousScreenName("sequencer");
        }
        else if (currentScreenName == "edit-sound")
        {
            const auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
            ls->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
        }
        else if (currentScreenName == "sound")
        {
            ls->setPreviousScreenName(sampler->getPreviousScreenName());
        }
        else if (currentScreenName == "program")
        {
            ls->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
            mpc.setPreviousSamplerScreenName("");
        }
        else if (currentScreenName == "name")
        {
            auto nameScreen = mpc.screens->get<NameScreen>("name");
            nameScreen->setEditing(false);
            ls->setLastFocus("name", "0");
        }
        else if (currentScreenName == "directory")
        {
            const auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
            ls->setPreviousScreenName(directoryScreen->previousScreenName);
        }
    }
    
    if (ls->getFocusedLayerIndex() == 1 ||
        ls->getFocusedLayerIndex() == 2 ||
        ls->getFocusedLayerIndex() == 3)
    {
        ls->openScreen(ls->getPreviousScreenName());
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

bool BaseControls::isTypable()
{
    return std::find(typableParams.begin(),
                     typableParams.end(),
                     param) != typableParams.end();
}

void BaseControls::numpad(int i)
{
    init();
    
    auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        auto field = ls->getFocusedLayer()->findField(param);
        
        if (isTypable())
        {
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
                ls->openScreen("vmpc-settings");
                break;
            case 1:
                if (sequencer.lock()->isPlaying())
                {
                    return;
                }
                
                ls->openScreen("song");
                break;
            case 2:
                ls->openScreen("punch");
                break;
            case 3:
            {
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("load");
                break;
            }
            case 4:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("sample");
                break;
            case 5:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("trim");
                break;
            case 6:
            {
                auto newDrum = sequencer.lock()->getActiveTrack()->getBus() - 1;

                if (newDrum >= 0)
                {
                    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                    drumScreen->setDrum(newDrum);
                }

                ls->openScreen("select-drum");
                break;
            }
            case 7:
            {
                auto newDrum = sequencer.lock()->getActiveTrack()->getBus() - 1;

                if (newDrum >= 0)
                {
                    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                    drumScreen->setDrum(newDrum);
                }

                ls->openScreen("select-mixer-drum");
                break;
            }
            case 8:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("others");
                break;
            case 9:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("sync");
                break;
        }
        
    }
}

void BaseControls::pressEnter()
{
    init();
    
    auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        ls->openScreen("save");
    }
}

void BaseControls::rec()
{
    init();

    if (collectionContainsCurrentScreen(screensThatOnlyAllowPlay))
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

    if (sequencer.lock()->isRecordingOrOverdubbing())
    {
        sequencer.lock()->setRecording(false);
        sequencer.lock()->setOverdubbing(false);
    }

    if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
    {
        ls->openScreen("sequencer");
    }
}

void BaseControls::overDub()
{
    init();

    if (collectionContainsCurrentScreen(screensThatOnlyAllowPlay))
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

    if (sequencer.lock()->isRecordingOrOverdubbing())
    {
        sequencer.lock()->setRecording(false);
        sequencer.lock()->setOverdubbing(false);
    }

    if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
    {
        ls->openScreen("sequencer");
    }
}

void BaseControls::stop()
{
    init();
    
    const auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
    const auto ams = mpc.getAudioMidiServices();
    const auto controls = mpc.getControls();
    
    if (controls->isNoteRepeatLocked())
    {
        controls->setNoteRepeatLocked(false);
    }
    
    if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->record != 4 || controls->isShiftPressed()))
        ams->stopBouncingEarly();
    
    sequencer.lock()->stop();
    
    if (!currentScreenAllowsPlay())
    {
        ls->openScreen("sequencer");
    }
}

void BaseControls::play()
{
    const auto controls = mpc.getControls();

    if (controls->isPlayPressed())
    {
        return;
    }

    controls->setPlayPressed(true);

    init();
    
    if (sequencer.lock()->isPlaying())
    {
        if (controls->isRecPressed() && !sequencer.lock()->isOverDubbing())
        {
            sequencer.lock()->setOverdubbing(false);
            sequencer.lock()->setRecording(true);
        }
        else if (controls->isOverDubPressed() && !sequencer.lock()->isRecording())
        {
            sequencer.lock()->setOverdubbing(true);
            sequencer.lock()->setRecording(false);
        }
    }
    else
    {
        if (controls->isRecPressed())
        {
            if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
            {
                ls->openScreen("sequencer");
            }
            
            sequencer.lock()->rec();
        }
        else if (controls->isOverDubPressed())
        {
            if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
            {
                ls->openScreen("sequencer");
            }
            
            sequencer.lock()->overdub();
        }
        else {
            if (controls->isShiftPressed() && !mpc.getAudioMidiServices()->isBouncing())
            {
                ls->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!currentScreenAllowsPlay())
                {
                    ls->openScreen("sequencer");
                }
                
                sequencer.lock()->setSongModeEnabled(currentScreenName == "song");
                sequencer.lock()->play();
            }
        }
    }
}

void BaseControls::playStart()
{
    init();

    if (sequencer.lock()->isPlaying())
    {
        return;
    }

    const auto controls = mpc.getControls();
    
    if (controls->isRecPressed())
    {
        if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
        {
            ls->openScreen("sequencer");
        }
        
        sequencer.lock()->recFromStart();
    }
    else if (controls->isOverDubPressed())
    {
        if (!collectionContainsCurrentScreen(screensThatAllowPlayAndRecord))
        {
            ls->openScreen("sequencer");
        }
        
        sequencer.lock()->overdubFromStart();
    }
    else
    {
        if (controls->isShiftPressed())
        {
            ls->openScreen("vmpc-direct-to-disk-recorder");
        }
        else
        {
            if (!currentScreenAllowsPlay())
            {
                ls->openScreen("sequencer");
            }
            
            sequencer.lock()->setSongModeEnabled(currentScreenName == "song");
            sequencer.lock()->playFromStart();
        }
    }
}

void BaseControls::mainScreen()
{
    init();
    
    const auto ams = mpc.getAudioMidiServices();
    
    if (ams->isRecordingSound())
    {
        ams->stopSoundRecorder();
    }

    ls->openScreen("sequencer");
    sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
    
    const auto hw = mpc.getHardware2();
    hw->getLed("next-seq")->setEnabled(false);
    hw->getLed("track-mute")->setEnabled(false);
}

void BaseControls::tap()
{
    init();
    const auto controls = mpc.getControls();

    if (controls->isTapPressed())
    {
        return;
    }

    controls->setTapPressed(true);
    sequencer.lock()->tap();
}

void BaseControls::goTo()
{
    if (!sequencer.lock()->getActiveSequence()->isUsed())
    {
        return;
    }

    init();

    const auto controls = mpc.getControls();
    controls->setGoToPressed(true);

    if (currentScreenName == "sequencer")
    {
        ls->openScreen("locate");
    }
}

void BaseControls::nextSeq()
{
    init();
    
    if (currentScreenName == "next-seq" ||
        currentScreenName == "next-seq-pad")
    {
        ls->openScreen("sequencer");
        mpc.getHardware2()->getLed("next-seq")->setEnabled(false);
    }
    else if (currentScreenName == "sequencer" ||
             currentScreenName == "track-mute")
    {
        Util::initSequence(mpc);
        ls->openScreen("next-seq");
        mpc.getHardware2()->getLed("next-seq")->setEnabled(true);
        mpc.getHardware2()->getLed("track-mute")->setEnabled(false);
    }
}

void BaseControls::trackMute()
{
    init();
    
    if (currentScreenName == "track-mute")
    {
        auto previous = ls->getPreviousScreenName();
        if (previous == "next-seq" || previous == "next-seq-pad")
        {
            ls->openScreen("next-seq");
        }
        else
        {
            ls->openScreen("sequencer");
        }
        
        mpc.getHardware2()->getLed("track-mute")->setEnabled(false);
    }
    else if
        (currentScreenName == "next-seq" ||
         currentScreenName == "next-seq-pad" ||
         currentScreenName == "sequencer")
    {
        Util::initSequence(mpc);
        ls->openScreen("track-mute");
        mpc.getHardware2()->getLed("track-mute")->setEnabled(true);
    }
}

void BaseControls::bank(int i)
{
    mpc.setBank(i);
}

void BaseControls::fullLevel()
{
    mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
    mpc.getHardware2()->getLed("full-level")->setEnabled(mpc.isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
    init();
    
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
        ls->openScreen("assign-16-levels");
    }
}

void BaseControls::after()
{
    init();
    
    const auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        ls->openScreen("assign");
    }
    else
    {
        mpc.setAfterEnabled(!mpc.isAfterEnabled());
        mpc.getHardware2()->getLed("after")->setEnabled(mpc.isAfterEnabled());
    }
}

void BaseControls::shift()
{
    const auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        return;
    }
    
    controls->setShiftPressed(true);
    
    init();

    const auto focus = ls->getFocusedLayer()->findField(param);
    
    if (focus && focus->isTypeModeEnabled())
    {
        focus->disableTypeMode();
        const auto split = focus->getActiveSplit();
        
        if (split != -1)
        {
            focus->setSplit(true);
            focus->setActiveSplit(split);
        }
    }
}

void BaseControls::undoSeq()
{
    sequencer.lock()->undoSeq();
}

void BaseControls::erase()
{
    init();

    const auto controls = mpc.getControls();
    controls->setErasePressed(true);
    
    if (!sequencer.lock()->getActiveSequence()->isUsed())
    {
        return;
    }
    
    if (!sequencer.lock()->isRecordingOrOverdubbing())
    {
        ls->openScreen("erase");
    }
}

int BaseControls::getSoundIncrement(int notch_inc)
{
    auto soundInc = notch_inc;
    
    if (abs(notch_inc) != 1)
    {
        soundInc *= (int)(ceil(sampler->getSound()->getFrameCount() / 15000.0));
    }
    
    return soundInc;
}

void BaseControls::splitLeft()
{
    init();
    
    const auto field = ls->getFocusedLayer()->findField(param);
    const auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        BaseControls::left();
        return;
    }
    
    if (!splittable)
    {
        return;
    }
    
    if (field->isSplit())
    {
        field->setActiveSplit(field->getActiveSplit() - 1);
    }
    else
    {
        field->setSplit(true);
    }
}

void BaseControls::splitRight()
{
    init();

    const auto field = ls->getFocusedLayer()->findField(param);
    const auto controls = mpc.getControls();
    
    if (!controls->isShiftPressed())
    {
        BaseControls::right();
        return;
    }
    
    if (splittable && field->isSplit())
    {
        if (!field->setActiveSplit(field->getActiveSplit() + 1))
        {
            field->setSplit(false);
        }
    }
}

bool BaseControls::currentScreenAllowsPlay()
{
    return collectionContainsCurrentScreen(screensThatOnlyAllowPlay) ||
           collectionContainsCurrentScreen(screensThatAllowPlayAndRecord);
}

bool BaseControls::collectionContainsCurrentScreen(const std::vector<std::string>& v)
{
    return find(v.begin(),
                v.end(),
                ls->getCurrentScreenName()) != v.end();
}

void BaseControls::handlePadHitInTrimLoopZoneParamsScreens()
{
    mpc.getBasicPlayer().mpcNoteOn(sampler->getSoundIndex(), 127, 0);
}

const std::vector<std::string> BaseControls::screensThatOnlyAllowPlay {
    "song",
    "track-mute",
    "next-seq",
    "next-seq-pad",
    "vmpc-recording-finished"
};

const std::vector<std::string> BaseControls::allowCentralNoteAndPadUpdateScreens{
    "program-assign",
    "program-params",
    "velocity-modulation",
    "velo-env-filter",
    "velo-pitch",
    "mute-assign",
    "assignment-view",
    "keep-or-retry",
    "load-a-sound"
};

const std::vector<std::string> BaseControls::screensThatAllowPlayAndRecord {
    "sequencer",
    "select-drum",
    "select-mixer-drum",
    "program-assign",
    "program-params",
    "drum", "purge",
    "program",
    "create-new-program",
    "name",
    "delete-program",
    "delete-all-programs",
    "assignment-view",
    "initialize-pad-assign",
    "copy-note-parameters",
    "velocity-modulation",
    "velo-env-filter",
    "velo-pitch",
    "mute-assign",
    "trans",
    "mixer",
    "mixer-setup",
    "channel-settings"
};

const std::vector<std::string> BaseControls::samplerScreens {
    "create-new-program",
    "assignment-view",
    "auto-chromatic-assignment",
    "copy-note-parameters",
    "edit-sound",
    "end-fine",
    "init-pad-assign",
    "keep-or-retry",
    "loop-end-fine",
    "loop-to-fine",
    "mute-assign",
    "program",
    "start-fine",
    "velo-env-filter",
    "velo-pitch",
    "velocity-modulation",
    "zone-end-fine",
    "zone-start-fine",
    "drum",
    "loop",
    "mixer",
    "mixer-setup",
    "channel-settings",
    "program-assign",
    "program-params",
    "select-drum",
    "trim",
    "zone",
    "load-a-sound"
};

const std::vector<std::string> BaseControls::soundScreens {
    // layer 1 (first layer, 1-based index, same level as Main screen
    "trim",
    "loop",
    "zone",
    "params",

    // layer 2
    "sound",
    "start-fine",
    "end-fine",
    "loop-to-fine",
    "loop-end-fine",
    "zone-start-fine",
    "zone-end-fine",
    "number-of-zones",
    "edit-sound",

    // layer 3
    "delete-sound",
    "delete-all-sound",
    "convert-sound",
    "stereo-to-mono",
    "mono-to-stereo",
    "resample"
};
