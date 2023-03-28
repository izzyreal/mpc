#include "BaseControls.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>

#include <sampler/Pad.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

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

#include "engine/Drum.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sequencer;

BaseControls::BaseControls(mpc::Mpc& _mpc)
: mpc (_mpc),
ls (_mpc.getLayeredScreen()),
sampler (_mpc.getSampler()),
sequencer (_mpc.getSequencer()),
mpcSoundPlayerChannel(_mpc.getDrum(0))
{
}

void BaseControls::init()
{
    currentScreenName = ls->getCurrentScreenName();
    param = ls->getFocus();
    activeField = ls->getFocusedLayer()->findField(param);
    
    auto isSampler = collectionContainsCurrentScreen(samplerScreens);
    
    if (isSampler)
    {
        splittable = param == "st" || param == "end" || param == "to" || param == "endlengthvalue" || param == "start";
    }
    else
    {
        splittable = false;
    }
    
    track = sequencer->getActiveTrack();
    
    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
    
    auto drumIndex = isSampler ? drumScreen->drum : track->getBus() - 1;
    
    if (drumIndex != -1)
    {
        mpcSoundPlayerChannel = sampler->getDrum(drumIndex);
        program = sampler->getProgram(mpcSoundPlayerChannel.getProgram());
    }
    else
    {
        program.reset();
    }
}

void BaseControls::left()
{
    init();
    
    if (!activeField)
        return;
    
    if (param == "dummy")
        return;
    
    ls->transferLeft();
}

void BaseControls::right()
{
    init();
    
    if (!activeField)
        return;
    
    if (param == "dummy")
        return;
    
    ls->transferRight();
}

void BaseControls::up()
{
    init();
    
    if (!activeField)
        return;
    
    if (param == "dummy")
        return;
    
    ls->transferUp();
}

void BaseControls::down()
{
    init();
    
    if (!activeField)
        return;
    
    if (param == "dummy")
        return;
    
    ls->transferDown();
}

void BaseControls::function(int i)
{
    init();
    
    switch (i)
    {
        case 3:
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
                    auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
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
                    auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
                    ls->setPreviousScreenName(directoryScreen->previousScreenName);
                }
            }
            
            if (ls->getFocusedLayerIndex() == 1 || ls->getFocusedLayerIndex() == 2 || ls->getFocusedLayerIndex() == 3)
                ls->openScreen(ls->getPreviousScreenName());
            break;
    }
}

void BaseControls::pad(int padIndexWithBank, int velocity)
{
    init();
    
    auto controls = mpc.getControls();
    auto hardware = mpc.getHardware();

    if (controls->isTapPressed() && sequencer->isPlaying())
    {
        return;
    }

    if (hardware->getTopPanel()->isFullLevelEnabled())
    {
        velocity = 127;
    }

    if (sequencer->isRecordingOrOverdubbing() && mpc.getControls()->isErasePressed())
        return;
    
    if (controls->isNoteRepeatLocked())
        return;
    
    auto note = track->getBus() > 0 ? program->getPad(padIndexWithBank)->getNote() : padIndexWithBank + 35;
    //auto velocity = velo;

    if (!mpc.getHardware()->getTopPanel()->isSixteenLevelsEnabled())
    {
        auto screenComponent = mpc.screens->getScreenComponent(currentScreenName);
        auto withNotes = std::dynamic_pointer_cast<WithTimesAndNotes>(screenComponent);
        auto assign16LevelsScreen = std::dynamic_pointer_cast<Assign16LevelsScreen>(screenComponent);
        auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(screenComponent);
        auto editMultipleScreen = std::dynamic_pointer_cast<EditMultipleScreen>(screenComponent);
        auto mixerScreen = std::dynamic_pointer_cast<MixerScreen>(screenComponent);
        auto channelSettingsScreen = std::dynamic_pointer_cast<ChannelSettingsScreen>(screenComponent);

        if (isDrumNote(note) && collectionContainsCurrentScreen(allowCentralNoteAndPadUpdateScreens))
        {
            mpc.setNote(note);
            mpc.setPad(padIndexWithBank);
        }
        else if (withNotes && isDrumNote(note))
        {
            withNotes->setNote0(note);
        }
        else if (assign16LevelsScreen)
        {
            assign16LevelsScreen->setNote(note);
        }
        else if (editMultipleScreen)
        {
            editMultipleScreen->setChangeNoteTo(note);
        }
        else if (stepEditorScreen && param == "fromnote" && note > 34)
        {
            stepEditorScreen->setFromNote(note);
        }
        else if (mixerScreen)
        {
            unsigned char bankStartPadIndex = mpc.getBank() * 16;
            unsigned char bankEndPadIndex = bankStartPadIndex + 16;
            if (padIndexWithBank >= bankStartPadIndex && padIndexWithBank < bankEndPadIndex)
            {
                mixerScreen->setXPos(padIndexWithBank % 16);
            }
        }
        else if (channelSettingsScreen)
        {
            channelSettingsScreen->setNote(note);
        }
    }
    
    generateNoteOn(note, velocity, padIndexWithBank);
}

void BaseControls::generateNoteOn(int note, int padVelo, int padIndexWithBank)
{
    init();
    
    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");

    bool isSliderNote = program && program->getSlider()->getNote() == note;
    
    bool posIsLastTick = sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();
    
    bool step = currentScreenName == "step-editor" && !posIsLastTick;
    
    auto tc_note = timingCorrectScreen->getNoteValue();
    auto tc_swing = timingCorrectScreen->getSwing();
    
    bool recMainWithoutPlaying = currentScreenName == "sequencer" &&
    !sequencer->isPlaying() &&
    mpc.getControls()->isRecPressed() &&
    tc_note != 0 &&
    !posIsLastTick;

    auto padIndex = program != nullptr ? program->getPadIndexFromNote(note) : - 1;

    std::shared_ptr<NoteOnEvent> noteOnEvent;

    if (sequencer->isRecordingOrOverdubbing() || step || recMainWithoutPlaying)
    {
        //std::shared_ptr<NoteOnEvent> recordedEvent;
        
        if (step)
        {
            if (track->getBus() == 0 || note >= 35)
            {
                noteOnEvent = track->addNoteEvent(sequencer->getTickPosition(), note);
            }
        }
        else if (recMainWithoutPlaying)
        {
            noteOnEvent = track->addNoteEvent(sequencer->getTickPosition(), note);
            int stepLength = sequencer->getTickValues()[tc_note];
            
            if (stepLength != 1)
            {
                int bar = sequencer->getCurrentBarIndex() + 1;
                track->timingCorrect(0, bar, noteOnEvent.get(), stepLength);
                
                std::vector<std::shared_ptr<Event>> events{ noteOnEvent };
                std::vector<int> noteRange {0, 127};
                track->swing(events, tc_note, tc_swing, noteRange);
                
                if (noteOnEvent->getTick() != sequencer->getTickPosition())
                    sequencer->move(noteOnEvent->getTick());
            }
        }
        else
        {
            noteOnEvent = track->recordNoteOnNow(note);
        }
        
        noteOnEvent->setVelocity(padVelo);
        noteOnEvent->setDuration(step ? 1 : NoteOnEvent::DURATION_UNKNOWN);

        if (step || recMainWithoutPlaying)
        {
            sequencer->playMetronomeTrack();
        }
    }
    
    auto play_event = std::make_shared<NoteOnEventPlayOnly>(note);
    play_event->setVelocity(padVelo);
    play_event->setTick(-1);
    
    if (program)
    {
        Util::set16LevelsValues(mpc, play_event, padIndex);

        if (isSliderNote)
        {
            Util::setSliderNoteVariationParameters(mpc, play_event, program);
        }
    }

    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
    char drum = collectionContainsCurrentScreen(samplerScreens) ? drumScreen->drum : -1;

    
    mpc.getEventHandler()->handle(play_event, track.get(), drum);
    
    assert(mpc.getControls()->storePlayNoteEvent(padIndexWithBank, play_event));
    
}

bool BaseControls::isTypable()
{
    for (auto str : typableParams)
    {
        if (str == param)
            return true;
    }
    
    return false;
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
                field->enableTypeMode();
            
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
                if (sequencer->isPlaying())
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
                if (sequencer->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("load");
                break;
            }
            case 4:
                if (sequencer->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("sample");
                break;
            case 5:
                if (sequencer->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("trim");
                break;
            case 6:
            {
                auto newDrum = sequencer->getActiveTrack()->getBus() - 1;
                if (newDrum < 0) newDrum = 0;
                auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                drumScreen->drum = newDrum;
                ls->openScreen("select-drum");
                break;
            }
            case 7:
            {
                auto newDrum = sequencer->getActiveTrack()->getBus() - 1;
                if (newDrum < 0) newDrum = 0;
                auto drumScreen = mpc.screens->get<DrumScreen>("drum");
                drumScreen->drum = newDrum;
                ls->openScreen("select-mixer-drum");
                break;
            }
            case 8:
                if (sequencer->isPlaying())
                {
                    break;
                }
                
                ls->openScreen("others");
                break;
            case 9:
                if (sequencer->isPlaying())
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
        ls->openScreen("save");
}

void BaseControls::rec()
{
    init();

    if (collectionContainsCurrentScreen(allowPlayScreens))
    {
        return;
    }

    auto controls = mpc.getControls();
    
    if (controls->isRecPressed())
    {
        return;
    }

    controls->setRecPressed(true);

    auto hw = mpc.getHardware();
    
    if (sequencer->isRecordingOrOverdubbing())
    {
        sequencer->setRecording(false);
        sequencer->setOverdubbing(false);
    }

    if (!collectionContainsCurrentScreen(allowTransportScreens))
    {
        ls->openScreen("sequencer");
    }
}

void BaseControls::overDub()
{
    init();

    if (collectionContainsCurrentScreen(allowPlayScreens))
    {
        return;
    }

    auto controls = mpc.getControls();
    controls->setOverDubPressed(true);

    auto hw = mpc.getHardware();
    

    if (sequencer->isRecordingOrOverdubbing())
    {
        sequencer->setRecording(false);
        sequencer->setOverdubbing(false);
    }

    if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
        ls->openScreen("sequencer");
}

void BaseControls::stop()
{
    init();
    
    auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
    auto ams = mpc.getAudioMidiServices();
    auto controls = mpc.getControls();
    
    if (controls->isNoteRepeatLocked())
        controls->setNoteRepeatLocked(false);
    
    if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->record != 4 || controls->isShiftPressed()))
        ams->stopBouncingEarly();
    
    sequencer->stop();
    
    if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
    {
        ls->openScreen("sequencer");
    }
}

void BaseControls::play()
{
    auto controls = mpc.getControls();

    if (controls->isPlayPressed())
    {
        return;
    }

    controls->setPlayPressed(true);

    init();
    auto hw = mpc.getHardware();
    
    if (sequencer->isPlaying())
    {
        if (controls->isRecPressed() && !sequencer->isOverDubbing())
        {
            sequencer->setOverdubbing(false);
            sequencer->setRecording(true);
        }
        else if (controls->isOverDubPressed() && !sequencer->isRecording())
        {
            sequencer->setOverdubbing(true);
            sequencer->setRecording(false);
        }
    }
    else
    {
        if (controls->isRecPressed())
        {
            if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
                ls->openScreen("sequencer");
            
            sequencer->rec();
        }
        else if (controls->isOverDubPressed())
        {
            if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
                ls->openScreen("sequencer");
            
            sequencer->overdub();
        }
        else {
            if (controls->isShiftPressed() && !mpc.getAudioMidiServices()->isBouncing())
            {
                ls->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
                {
                    ls->openScreen("sequencer");
                }
                
                sequencer->setSongModeEnabled(currentScreenName == "song");
                sequencer->play();
            }
        }
    }
}

void BaseControls::playStart()
{
    init();
    auto hw = mpc.getHardware();
    auto controls = mpc.getControls();
    
    if (sequencer->isPlaying())
    {
        return;
    }
    
    if (controls->isRecPressed())
    {
        if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
            ls->openScreen("sequencer");
        
        sequencer->recFromStart();
    }
    else if (controls->isOverDubPressed())
    {
        if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
            ls->openScreen("sequencer");
        
        sequencer->overdubFromStart();
    }
    else
    {
        if (controls->isShiftPressed())
        {
            ls->openScreen("vmpc-direct-to-disk-recorder");
        }
        else
        {
            if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
            {
                ls->openScreen("sequencer");
            }
            
            sequencer->setSongModeEnabled(currentScreenName == "song");
            sequencer->playFromStart();
        }
    }
}

void BaseControls::mainScreen()
{
    init();
    
    auto ams = mpc.getAudioMidiServices();
    
    if (ams->isRecordingSound())
        ams->stopSoundRecorder();
    
    ls->openScreen("sequencer");
    sequencer->setSoloEnabled(sequencer->isSoloEnabled());
    
    auto hw = mpc.getHardware();
    hw->getLed("next-seq")->light(false);
    hw->getLed("track-mute")->light(false);
}

void BaseControls::tap()
{
    init();
    auto controls = mpc.getControls();
    controls->setTapPressed(true);
    sequencer->tap();
}

void BaseControls::goTo()
{
    init();
    auto controls = mpc.getControls();
    controls->setGoToPressed(true);
}

void BaseControls::nextSeq()
{
    init();
    
    if (currentScreenName == "next-seq" ||
        currentScreenName == "next-seq-pad")
    {
        ls->openScreen("sequencer");
        mpc.getHardware()->getLed("next-seq")->light(false);
    }
    else if (currentScreenName == "sequencer" ||
             currentScreenName == "track-mute")
    {
        Util::initSequence(mpc);
        ls->openScreen("next-seq");
        mpc.getHardware()->getLed("next-seq")->light(true);
        mpc.getHardware()->getLed("track-mute")->light(false);
    }
}

void BaseControls::trackMute()
{
    init();
    
    if (currentScreenName == "track-mute")
    {
        auto previous = ls->getPreviousScreenName();
        if (previous == "next-seq" || previous == "next-seq-pad")
            ls->openScreen("next-seq");
        else
            ls->openScreen("sequencer");
        
        mpc.getHardware()->getLed("track-mute")->light(false);
    }
    else if
        (currentScreenName == "next-seq" ||
         currentScreenName == "next-seq-pad" ||
         currentScreenName == "sequencer")
    {
        Util::initSequence(mpc);
        ls->openScreen("track-mute");
        mpc.getHardware()->getLed("track-mute")->light(true);
    }
}

void BaseControls::bank(int i)
{
    mpc.setBank(i);

    for (int p = 0; p < 16; p++)
    {
      mpc.getHardware()->getPad(p)->notifyObservers(255);
    }
}

void BaseControls::fullLevel()
{
    init();
    auto hardware = mpc.getHardware();
    auto topPanel = hardware->getTopPanel();
    
    topPanel->setFullLevelEnabled(!topPanel->isFullLevelEnabled());
    
    hardware->getLed("full-level")->light(topPanel->isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
    init();
    
    if (currentScreenName != "sequencer" &&
        currentScreenName != "assign-16-levels")
    {
        return;
    }
    
    auto hardware = mpc.getHardware();
    auto topPanel = hardware->getTopPanel();
    
    if (topPanel->isSixteenLevelsEnabled())
    {
        topPanel->setSixteenLevelsEnabled(false);
        hardware->getLed("sixteen-levels")->light(false);
    }
    else {
        ls->openScreen("assign-16-levels");
    }
}

void BaseControls::after()
{
    init();
    auto hw = mpc.getHardware();
    auto topPanel = hw->getTopPanel();
    auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
    {
        ls->openScreen("assign");
    }
    else
    {
        topPanel->setAfterEnabled(!topPanel->isAfterEnabled());
        hw->getLed("after")->light(topPanel->isAfterEnabled());
    }
}

void BaseControls::shift()
{
    auto controls = mpc.getControls();
    
    if (controls->isShiftPressed())
        return;
    
    controls->setShiftPressed(true);
    
    init();
    auto focus = ls->getFocusedLayer()->findField(param);
    
    if (focus && focus->isTypeModeEnabled())
    {
        focus->disableTypeMode();
        auto split = focus->getActiveSplit();
        
        if (split != -1)
        {
            focus->setSplit(true);
            focus->setActiveSplit(split);
        }
    }
}

void BaseControls::undoSeq()
{
    sequencer->undoSeq();
}

void BaseControls::erase()
{
    init();
    auto controls = mpc.getControls();
    controls->setErasePressed(true);
    
    if (!sequencer->getActiveSequence()->isUsed())
    {
        return;
    }
    
    if (!sequencer->isRecordingOrOverdubbing())
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
    
    auto field = ls->getFocusedLayer()->findField(param);
    auto controls = mpc.getControls();
    
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
    auto field = ls->getFocusedLayer()->findField(param);
    auto controls = mpc.getControls();
    
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

bool BaseControls::collectionContainsCurrentScreen(const std::vector<std::string>& v)
{
    return find(
            v.begin(),
            v.end(),
            ls->getCurrentScreenName()
    ) != v.end();
}

const std::vector<std::string> BaseControls::allowPlayScreens {
    "song",
    "track-mute",
    "next-seq",
    "next-seq-pad"
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

const std::vector<std::string> BaseControls::allowTransportScreens {
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
    "mixer-setup"
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
    "program-assign",
    "program-params",
    "select-drum",
    "trim",
    "zone"
};
