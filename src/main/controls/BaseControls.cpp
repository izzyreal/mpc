#include "BaseControls.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>

#include <disk/AbstractDisk.hpp>

#include <sampler/Pad.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

#include <lcdgui/Layer.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <lcdgui/screens/window/EditMultipleScreen.hpp>

#include <Util.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sequencer;
using namespace std;

BaseControls::BaseControls(mpc::Mpc& _mpc)
: mpc (_mpc),
ls (_mpc.getLayeredScreen()),
sampler (_mpc.getSampler()),
sequencer (_mpc.getSequencer())
{
}

void BaseControls::init()
{
    currentScreenName = ls.lock()->getCurrentScreenName();
    param = ls.lock()->getFocus();
    activeField = ls.lock()->getFocusedLayer().lock()->findField(param);
    
    auto isSampler = isSamplerScreen();
    
    if (isSampler)
    {
        splittable = param == "st" || param == "end" || param == "to" || param == "endlengthvalue" || param == "start";
    }
    else
    {
        splittable = false;
    }
    
    track = sequencer.lock()->getActiveTrack();
    
    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
    
    auto drumIndex = isSampler ? drumScreen->drum : track.lock()->getBus() - 1;
    
    if (drumIndex != -1)
    {
        mpcSoundPlayerChannel = sampler.lock()->getDrum(drumIndex);
        program = sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram());
    }
    else
    {
        mpcSoundPlayerChannel = nullptr;
        program.reset();
    }
}

void BaseControls::left()
{
    init();
    
    if (!activeField.lock())
        return;
    
    if (param == "dummy")
        return;
    
    ls.lock()->transferLeft();
}

void BaseControls::right()
{
    init();
    
    if (!activeField.lock())
        return;
    
    if (param == "dummy")
        return;
    
    ls.lock()->transferRight();
}

void BaseControls::up()
{
    init();
    
    if (!activeField.lock())
        return;
    
    if (param == "dummy")
        return;
    
    ls.lock()->transferUp();
}

void BaseControls::down()
{
    init();
    
    if (!activeField.lock())
        return;
    
    if (param == "dummy")
        return;
    
    ls.lock()->transferDown();
}

void BaseControls::function(int i)
{
    init();
    
    switch (i)
    {
        case 3:
            auto controls = mpc.getControls().lock();
            controls->setF4Pressed(true);
            if (ls.lock()->getFocusedLayerIndex() == 1)
            {
                if (currentScreenName == "sequence")
                {
                    ls.lock()->setPreviousScreenName("sequencer");
                }
                else if (currentScreenName == "midi-input")
                {
                    ls.lock()->setPreviousScreenName("sequencer");
                }
                else if (currentScreenName == "midi-output")
                {
                    ls.lock()->setPreviousScreenName("sequencer");
                }
                else if (currentScreenName == "edit-sound")
                {
                    auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
                    ls.lock()->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
                }
                else if (currentScreenName == "sound")
                {
                    ls.lock()->setPreviousScreenName(sampler.lock()->getPreviousScreenName());
                }
                else if (currentScreenName == "program")
                {
                    ls.lock()->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
                }
                else if (currentScreenName == "name")
                {
                    auto nameScreen = mpc.screens->get<NameScreen>("name");
                    nameScreen->editing = false;
                    ls.lock()->setLastFocus("name", "0");
                }
                else if (currentScreenName == "directory")
                {
                    auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
                    ls.lock()->setPreviousScreenName(directoryScreen->previousScreenName);
                }
            }
            
            if (ls.lock()->getFocusedLayerIndex() == 1 || ls.lock()->getFocusedLayerIndex() == 2 || ls.lock()->getFocusedLayerIndex() == 3)
                ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
            break;
    }
}

void BaseControls::pad(int padIndexWithBank, int velo, bool triggeredByRepeat, int tick)
{
    init();
    
    auto controls = mpc.getControls().lock();
    auto hardware = mpc.getHardware().lock();

    if (hardware->getTopPanel().lock()->isFullLevelEnabled())
    {
      velo = 127;
    }

    if (sequencer.lock()->isRecordingOrOverdubbing() && mpc.getControls().lock()->isErasePressed())
        return;
    
    if (controls->isNoteRepeatLocked() && !triggeredByRepeat)
        return;
    
    auto note = track.lock()->getBus() > 0 ? program.lock()->getPad(padIndexWithBank)->getNote() : padIndexWithBank + 35;
    auto velocity = velo;

    if (!mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
    {
        auto withNotes = std::dynamic_pointer_cast<WithTimesAndNotes>(mpc.screens->getScreenComponent(currentScreenName));
        auto assign16LevelsScreen = std::dynamic_pointer_cast<Assign16LevelsScreen>(mpc.screens->getScreenComponent(currentScreenName));
        auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(mpc.screens->getScreenComponent(currentScreenName));
        auto editMultipleScreen = std::dynamic_pointer_cast<EditMultipleScreen>(mpc.screens->getScreenComponent(currentScreenName));

        if (note >= 35 && note <= 98 && collectionContainsCurrentScreen(allowCentralNoteAndPadUpdateScreens))
        {
            mpc.setNote(note);
            mpc.setPad(padIndexWithBank);
        }
        else if (withNotes && note >= 35)
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
    }
    
    if (controls->isTapPressed() && sequencer.lock()->isPlaying())
    {
        if (triggeredByRepeat)
            generateNoteOn(note, velocity, tick);
    }
    else
    {
        generateNoteOn(note, velocity, -1);
    }
}

void BaseControls::generateNoteOn(int note, int padVelo, int tick)
{
    init();
    
    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
    
    auto pgm = program.lock();
    auto seq = sequencer.lock();
    auto trk = track.lock();
    
    bool isSliderNote = pgm && pgm->getSlider()->getNote() == note;
    
    bool posIsLastTick = seq->getTickPosition() == seq->getActiveSequence().lock()->getLastTick();
    
    bool step = currentScreenName == "step-editor" && !posIsLastTick;
    
    auto tc_note = timingCorrectScreen->getNoteValue();
    auto tc_swing = timingCorrectScreen->getSwing();
    
    bool recMainWithoutPlaying = currentScreenName == "sequencer" &&
    !seq->isPlaying() &&
    mpc.getControls().lock()->isRecPressed() &&
    tc_note != 0 &&
    !posIsLastTick;
    
    auto padIndex = program.lock()->getPadIndexFromNote(note);
    
    if (seq->isRecordingOrOverdubbing() || step || recMainWithoutPlaying)
    {
        shared_ptr<NoteEvent> recordedEvent;
        
        if (step)
        {
            if (trk->getBus() == 0 || note >= 35)
            {
                recordedEvent = trk->addNoteEvent(seq->getTickPosition(), note).lock();
            }
        }
        else if (recMainWithoutPlaying)
        {
            recordedEvent = trk->addNoteEvent(seq->getTickPosition(), note).lock();
            int stepLength = seq->getTickValues()[tc_note];
            
            if (stepLength != 1)
            {
                int bar = seq->getCurrentBarIndex() + 1;
                trk->timingCorrect(0, bar, recordedEvent.get(), stepLength);
                
                vector<weak_ptr<Event>> events{ recordedEvent };
                trk->swing(events, tc_note, tc_swing, vector<int>{0, 127});
                
                if (recordedEvent->getTick() != seq->getTickPosition())
                    seq->move(recordedEvent->getTick());
            }
        }
        else
        {
            recordedEvent = trk->recordNoteOn().lock();
            
            if (recordedEvent)
                recordedEvent->setNote(note);
        }
        
        if (recordedEvent)
        {
            recordedEvent->setVelocity(padVelo);
            recordedEvent->setDuration(step ? 1 : -1);
            Util::set16LevelsValues(mpc, recordedEvent, padIndex);
            
            if (isSliderNote)
                Util::setSliderNoteVariationParameters(mpc, recordedEvent, program);

            if (step || recMainWithoutPlaying)
                seq->playMetronomeTrack();
        }
    }
    
    auto playableEvent = make_shared<NoteEvent>(note);
    playableEvent->setVelocity(padVelo);
    
    Util::set16LevelsValues(mpc, playableEvent, padIndex);
    
    if (isSliderNote)
        Util::setSliderNoteVariationParameters(mpc, playableEvent, program);
    
    playableEvent->setDuration(0);
    playableEvent->setTick(tick);
    
    mpc.getEventHandler().lock()->handle(playableEvent, trk.get());
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
    
    auto controls = mpc.getControls().lock();
    
    if (!controls->isShiftPressed())
    {
        auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
        
        if (isTypable())
        {
            if (!field->isTypeModeEnabled())
                field->enableTypeMode();
            
            field->type(i);
        }
    }
    
    auto disk = mpc.getDisk().lock();
    
    if (controls->isShiftPressed())
    {
        switch (i)
        {
            case 0:
                ls.lock()->openScreen("vmpc-settings");
                break;
            case 1:
                if (sequencer.lock()->isPlaying())
                {
                    return;
                }
                
                ls.lock()->openScreen("song");
                break;
            case 2:
                ls.lock()->openScreen("punch");
                break;
            case 3:
            {
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                disk->initFiles();
                
                ls.lock()->openScreen("load");
                break;
            }
            case 4:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls.lock()->openScreen("sample");
                break;
            case 5:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls.lock()->openScreen("trim");
                break;
            case 6:
                ls.lock()->openScreen("select-drum");
                break;
            case 7:
                ls.lock()->openScreen("select-mixer-drum");
                break;
            case 8:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls.lock()->openScreen("others");
                break;
            case 9:
                if (sequencer.lock()->isPlaying())
                {
                    break;
                }
                
                ls.lock()->openScreen("sync");
                break;
        }
        
    }
}

void BaseControls::pressEnter()
{
    init();
    
    auto controls = mpc.getControls().lock();
    
    if (controls->isShiftPressed())
        ls.lock()->openScreen("save");
}

void BaseControls::rec()
{
    init();

    if (collectionContainsCurrentScreen(allowPlayScreens))
    {
        return;
    }

    auto controls = mpc.getControls().lock();
    
    if (controls->isRecPressed())
    {
        return;
    }
    
    controls->setRecPressed(true);

    auto hw = mpc.getHardware().lock();
    
    if (sequencer.lock()->isRecordingOrOverdubbing())
    {
        sequencer.lock()->setRecording(false);
        sequencer.lock()->setOverdubbing(false);
    }

    if (!collectionContainsCurrentScreen(allowTransportScreens))
    {
        ls.lock()->openScreen("sequencer");
    }
}

void BaseControls::overDub()
{
    init();

    if (collectionContainsCurrentScreen(allowPlayScreens))
    {
        return;
    }

    auto controls = mpc.getControls().lock();
    controls->setOverDubPressed(true);

    auto hw = mpc.getHardware().lock();
    

    if (sequencer.lock()->isRecordingOrOverdubbing())
    {
        sequencer.lock()->setRecording(false);
        sequencer.lock()->setOverdubbing(false);
    }

    if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
        ls.lock()->openScreen("sequencer");
}

void BaseControls::stop()
{
    init();
    
    auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
    auto ams = mpc.getAudioMidiServices().lock();
    auto controls = mpc.getControls().lock();
    
    if (controls->isNoteRepeatLocked())
        controls->setNoteRepeatLocked(false);
    
    if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->record != 4 || controls->isShiftPressed()))
        ams->stopBouncingEarly();
    
    sequencer.lock()->stop();
    
    if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
    {
        ls.lock()->openScreen("sequencer");
    }
}

void BaseControls::play()
{
    auto controls = mpc.getControls().lock();

    if (controls->isPlayPressed())
    {
        return;
    }

    controls->setPlayPressed(true);

    init();
    auto hw = mpc.getHardware().lock();
    
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
            if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
                ls.lock()->openScreen("sequencer");
            
            sequencer.lock()->rec();
        }
        else if (controls->isOverDubPressed())
        {
            if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
                ls.lock()->openScreen("sequencer");
            
            sequencer.lock()->overdub();
        }
        else {
            if (controls->isShiftPressed() && !mpc.getAudioMidiServices().lock()->isBouncing())
            {
                ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
                {
                    ls.lock()->openScreen("sequencer");
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
    auto hw = mpc.getHardware().lock();
    auto controls = mpc.getControls().lock();
    
    if (sequencer.lock()->isPlaying())
    {
        return;
    }
    
    if (controls->isRecPressed())
    {
        if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
            ls.lock()->openScreen("sequencer");
        
        sequencer.lock()->recFromStart();
        
        if (!sequencer.lock()->isRecording())
            return;
    }
    else if (controls->isOverDubPressed())
    {
        if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
            ls.lock()->openScreen("sequencer");
        
        sequencer.lock()->overdubFromStart();
        
        if (!sequencer.lock()->isOverDubbing())
            return;
    }
    else
    {
        if (controls->isShiftPressed())
        {
            ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
        }
        else
        {
            if (!collectionContainsCurrentScreen(allowPlayScreens) && !collectionContainsCurrentScreen(allowTransportScreens))
            {
                ls.lock()->openScreen("sequencer");
            }
            
            sequencer.lock()->setSongModeEnabled(currentScreenName == "song");
            sequencer.lock()->playFromStart();
        }
    }
}

void BaseControls::mainScreen()
{
    init();
    
    auto ams = mpc.getAudioMidiServices().lock();
    
    if (ams->isRecordingSound())
        ams->stopSoundRecorder();
    
    ls.lock()->openScreen("sequencer");
    sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
    
    auto hw = mpc.getHardware().lock();
    hw->getLed("next-seq").lock()->light(false);
    hw->getLed("track-mute").lock()->light(false);
}

void BaseControls::tap()
{
    init();
    auto controls = mpc.getControls().lock();
    controls->setTapPressed(true);
    sequencer.lock()->tap();
}

void BaseControls::goTo()
{
    init();
    auto controls = mpc.getControls().lock();
    controls->setGoToPressed(true);
}

void BaseControls::nextSeq()
{
    init();
    
    if (currentScreenName == "next-seq" ||
        currentScreenName == "next-seq-pad")
    {
        ls.lock()->openScreen("sequencer");
        mpc.getHardware().lock()->getLed("next-seq").lock()->light(false);
    }
    else if (currentScreenName == "sequencer" ||
             currentScreenName == "track-mute")
    {
        Util::initSequence(mpc);
        ls.lock()->openScreen("next-seq");
        mpc.getHardware().lock()->getLed("next-seq").lock()->light(true);
        mpc.getHardware().lock()->getLed("track-mute").lock()->light(false);
    }
}

void BaseControls::trackMute()
{
    init();
    
    if (currentScreenName == "track-mute")
    {
        auto previous = ls.lock()->getPreviousScreenName();
        if (previous == "next-seq" || previous == "next-seq-pad")
            ls.lock()->openScreen("next-seq");
        else
            ls.lock()->openScreen("sequencer");
        
        mpc.getHardware().lock()->getLed("track-mute").lock()->light(false);
    }
    else if
        (currentScreenName == "next-seq" ||
         currentScreenName == "next-seq-pad" ||
         currentScreenName == "sequencer")
    {
        Util::initSequence(mpc);
        ls.lock()->openScreen("track-mute");
        mpc.getHardware().lock()->getLed("track-mute").lock()->light(true);
    }
}

void BaseControls::bank(int i)
{
    mpc.setBank(i);

    for (int p = 0; p < 16; p++)
    {
      mpc.getHardware().lock()->getPad(p).lock()->notifyObservers(255);
    }
}

void BaseControls::fullLevel()
{
    init();
    auto hardware = mpc.getHardware().lock();
    auto topPanel = hardware->getTopPanel().lock();
    
    topPanel->setFullLevelEnabled(!topPanel->isFullLevelEnabled());
    
    hardware->getLed("full-level").lock()->light(topPanel->isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
    init();
    
    if (currentScreenName != "sequencer" &&
        currentScreenName != "assign-16-levels")
    {
        return;
    }
    
    auto hardware = mpc.getHardware().lock();
    auto topPanel = hardware->getTopPanel().lock();
    
    if (topPanel->isSixteenLevelsEnabled())
    {
        topPanel->setSixteenLevelsEnabled(false);
        hardware->getLed("sixteen-levels").lock()->light(false);
    }
    else {
        ls.lock()->openScreen("assign-16-levels");
    }
}

void BaseControls::after()
{
    init();
    auto hw = mpc.getHardware().lock();
    auto topPanel = hw->getTopPanel().lock();
    auto controls = mpc.getControls().lock();
    
    if (controls->isShiftPressed())
    {
        ls.lock()->openScreen("assign");
    }
    else
    {
        topPanel->setAfterEnabled(!topPanel->isAfterEnabled());
        hw->getLed("after").lock()->light(topPanel->isAfterEnabled());
    }
}

void BaseControls::shift()
{
    auto controls = mpc.getControls().lock();
    
    if (controls->isShiftPressed())
        return;
    
    controls->setShiftPressed(true);
    
    init();
    auto focus = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
    
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
    sequencer.lock()->undoSeq();
}

void BaseControls::erase()
{
    init();
    auto controls = mpc.getControls().lock();
    controls->setErasePressed(true);
    
    if (!sequencer.lock()->getActiveSequence().lock()->isUsed())
    {
        return;
    }
    
    if (!sequencer.lock()->isRecordingOrOverdubbing())
    {
        ls.lock()->openScreen("erase");
    }
}

int BaseControls::getSoundIncrement(int notch_inc)
{
    auto soundInc = notch_inc;
    
    if (abs(notch_inc) != 1)
    {
        soundInc *= (int)(ceil(sampler.lock()->getSound().lock()->getFrameCount() / 15000.0));
    }
    
    return soundInc;
}

void BaseControls::splitLeft()
{
    init();
    
    auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
    auto controls = mpc.getControls().lock();
    
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
    auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
    auto controls = mpc.getControls().lock();
    
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
            ls.lock()->getCurrentScreenName()
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
    "trans"
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

bool BaseControls::isSamplerScreen()
{
    return find(
                begin(BaseControls::samplerScreens),
                end(BaseControls::samplerScreens),
                ls.lock()->getCurrentScreenName()
                ) != end(BaseControls::samplerScreens);
}
