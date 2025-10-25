#include "SequencerScreen.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"

#include "sequencer/Track.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/TimeSignature.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/PunchScreen.hpp"
#include "lcdgui/PunchRect.hpp"

#include "sequencer/SeqUtil.hpp"
#include <Util.hpp>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

SequencerScreen::SequencerScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sequencer", layerIndex)
{
    MRECT punch0(0, 52, 30, 59);
    addChildT<PunchRect>("punch-rect-0", punch0)->Hide(true);

    MRECT punch1(105, 52, 135, 59);
    addChildT<PunchRect>("punch-rect-1", punch1)->Hide(true);

    MRECT punch2(217, 52, 247, 59);
    addChildT<PunchRect>("punch-rect-2", punch2)->Hide(true);

    addChildT<TextComp>(mpc, "footer-label");
    auto footerLabel = findChild<TextComp>("footer-label");
    footerLabel->setLocation(36, 51);
    footerLabel->setText("(Hold pads or keys to repeat)");
    footerLabel->setSize(footerLabel->GetTextEntryLength() * 6, 8);
    footerLabel->Hide(true);
}

void SequencerScreen::open()
{
    findField("loop")->setAlignment(Alignment::Centered);
    findField("on")->setAlignment(Alignment::Centered);
    findField("bars")->setAlignment(Alignment::Centered);
    findField("pgm")->setAlignment(Alignment::Centered);
    findField("count")->setAlignment(Alignment::Centered);

    findLabel("tempo")->setSize(12, 9);
    findField("tempo")->setLocation(18, 11);
    findField("tempo")->setLeftMargin(1);

    sequence = sequencer.lock()->getActiveSequence();

    findLabel("punch-time-0")->Hide(true);
    findLabel("punch-time-1")->Hide(true);

    displaySq();
    displayTr();
    displayOn();
    displayCount();
    displayTiming();
    displayLoop();
    displayRecordingMode();
    displayBars();
    displayNow0();
    displayNow1();
    displayNow2();
    displayTempo();
    displayTempoSource();
    displayTsig();
    displayPgm();
    displayVelo();
    displayBus();
    displayDeviceNumber();
    displayNextSq();

    sequencer.lock()->addObserver(this);
    sequence.lock()->addObserver(this);

    for (auto &t : sequence.lock()->getTracks())
    {
        t->addObserver(this);
    }

    findChild<TextComp>("fk3")->setBlinking(sequencer.lock()->isSoloEnabled());

    auto punchScreen = mpc.screens->get<PunchScreen>();

    if (sequencer.lock()->isSecondSequenceEnabled())
    {
        findBackground()->setBackgroundName("sequencer-2nd");
    }
    else if (sequencer.lock()->isPunchEnabled() && !sequencer.lock()->isRecordingOrOverdubbing())
    {
        findBackground()->setBackgroundName("sequencer-punch-active");
    }
    else
    {
        findBackground()->setBackgroundName("sequencer");
    }

    if (sequencer.lock()->getNextSq() != -1)
    {
        ls->setFocus("nextsq");
    }

    const bool sequencerIsRecordingOrOverdubbing = sequencer.lock()->isRecordingOrOverdubbing();

    const auto footerIsInvisible = !mpc.clientEventController->clientHardwareEventController->isNoteRepeatLocked() &&
                                   !(mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed() &&
                                     sequencerIsRecordingOrOverdubbing);

    findChild("footer-label")->Hide(footerIsInvisible);

    findChild("function-keys")->Hide(!footerIsInvisible || sequencer.lock()->isPunchEnabled() || (mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed() && sequencerIsRecordingOrOverdubbing));
}

void SequencerScreen::erase()
{
    findChild("function-keys")->Hide(true);
    findChild("footer-label")->Hide(false);
    findChild<TextComp>("footer-label")->setText("(Hold pads or keys to erase)");
}

void SequencerScreen::tap()
{
    if (sequencer.lock()->isPlaying())
    {
        if (mpc.clientEventController->clientHardwareEventController->isNoteRepeatLocked())
        {
            findChild("function-keys")->Hide(false);
            findChild("footer-label")->Hide(true);
        }
        else
        {
            findChild("function-keys")->Hide(true);
            findChild("footer-label")->Hide(false);
            findChild<TextComp>("footer-label")->setText("(Hold pads or keys to repeat)");
        }
    }
}

void SequencerScreen::hideFooterLabelAndShowFunctionKeys()
{
    findChild("footer-label")->Hide(true);
    findChild("function-keys")->Hide(false);
}

void SequencerScreen::close()
{
    std::vector<std::string> screensThatDisablePunch{"song", "load", "save", "others", "next-seq"};
    auto nextScreen = ls->getCurrentScreenName();

    if (find(begin(screensThatDisablePunch), end(screensThatDisablePunch), nextScreen) != end(screensThatDisablePunch))
    {
        sequencer.lock()->setPunchEnabled(false);
    }

    sequencer.lock()->resetUndo();

    sequencer.lock()->deleteObserver(this);
    sequence.lock()->deleteObserver(this);
    for (auto &t : sequence.lock()->getTracks())
    {
        t->deleteObserver(this);
    }
}

void SequencerScreen::displayVelo()
{
    findField("velo")->setTextPadded(std::to_string(sequencer.lock()->getActiveTrack()->getVelocityRatio()));
}

void SequencerScreen::displayDeviceNumber()
{
    auto track = mpc.getSequencer()->getActiveTrack();
    if (track->getDeviceIndex() == 0)
    {
        findField("devicenumber")->setText("OFF");
    }
    else
    {
        if (track->getDeviceIndex() >= 17)
        {
            findField("devicenumber")->setText(std::to_string(track->getDeviceIndex() - 16) + "B");
        }
        else
        {
            findField("devicenumber")->setText(std::to_string(track->getDeviceIndex()) + "A");
        }
    }
}

std::vector<std::string> SequencerScreen::busNames = std::vector<std::string>{"MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4"};

void SequencerScreen::displayBus()
{
    findField("bus")->setText(busNames[sequencer.lock()->getActiveTrack()->getBus()]);
    displayDeviceName();
}

void SequencerScreen::displayBars()
{
    findField("bars")->setText(std::to_string(sequencer.lock()->getActiveSequence()->getLastBarIndex() + 1));
}

void SequencerScreen::displayPgm()
{
    auto track = mpc.getSequencer()->getActiveTrack();
    if (track->getProgramChange() == 0)
    {
        findField("pgm")->setText("OFF");
    }
    else
    {
        findField("pgm")->setText(std::to_string(track->getProgramChange()));
    }
}

void SequencerScreen::displayDeviceName()
{
    auto track = mpc.getSequencer()->getActiveTrack();

    if (track->getBus() != 0)
    {
        if (track->getDeviceIndex() == 0)
        {
            int pgm = sampler->getDrumBusProgramIndex(track->getBus());
            auto p = sampler->getProgram(pgm);
            findLabel("devicename")->setText(p->getName());
        }
        else
        {
            findLabel("devicename")->setText(sequencer.lock()->getActiveSequence()->getDeviceName(track->getDeviceIndex()));
        }
    }
    else if (track->getBus() == 0)
    {
        if (track->getDeviceIndex() == 0)
        {
            findLabel("devicename")->setText("NewPgm-A");
        }
        else
        {
            findLabel("devicename")->setText(sequencer.lock()->getActiveSequence()->getDeviceName(track->getDeviceIndex()));
        }
    }
}

void SequencerScreen::displayTempo()
{
    displayTempoLabel();
    findField("tempo")->setText(StrUtil::padLeft(Util::tempoString(sequencer.lock()->getTempo()), " ", 6));
}

void SequencerScreen::displayTempoLabel()
{
    auto currentRatio = -1;
    auto seq = sequencer.lock()->getActiveSequence();

    if (!seq->isUsed() || !seq->isTempoChangeOn())
    {
        findLabel("tempo")->setText(u8" \u00C0:");
        return;
    }

    for (auto &tce : seq->getTempoChangeEvents())
    {
        if (tce->getTick() > sequencer.lock()->getTickPosition())
        {
            break;
        }

        currentRatio = tce->getRatio();
    }

    if (currentRatio != 1000)
    {
        findLabel("tempo")->setText(u8"c\u00C0:");
    }
    else
    {
        findLabel("tempo")->setText(u8" \u00C0:");
    }
}

void SequencerScreen::displayTempoSource()
{
    findField("tempo-source")->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}

void SequencerScreen::displaySq()
{
    std::string result;

    if (sequencer.lock()->isPlaying())
    {
        result.append(StrUtil::padLeft(std::to_string(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
        result.append("-");
        result.append(sequencer.lock()->getCurrentlyPlayingSequence()->getName());
        findField("sq")->setText(result);
    }
    else
    {
        result.append(StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
        result.append("-");
        result.append(sequencer.lock()->getActiveSequence()->getName());
        findField("sq")->setText(result);
    }
}

void SequencerScreen::displayNow0()
{
    findField("now0")->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void SequencerScreen::displayNow1()
{
    findField("now1")->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void SequencerScreen::displayNow2()
{
    findField("now2")->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SequencerScreen::displayRecordingMode()
{
    findField("recordingmode")->setText(sequencer.lock()->isRecordingModeMulti() ? "M" : "S");
}

void SequencerScreen::displayTsig()
{
    std::string result;
    auto ts = sequence.lock()->getTimeSignature();
    result.append(std::to_string(ts.getNumerator()));
    result.append("/");
    result.append(std::to_string(ts.getDenominator()));
    findField("tsig")->setText(mpc::Util::distributeTimeSig(result));
}

void SequencerScreen::displayLoop()
{
    findField("loop")->setText(sequence.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void SequencerScreen::displayOn()
{
    findField("on")->setText(sequencer.lock()->getActiveTrack()->isOn() ? "YES" : "NO");
}

void SequencerScreen::displayTr()
{

    auto result = StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveTrackIndex() + 1), "0", 2);
    result.append("-");
    result.append(sequencer.lock()->getActiveTrack()->getName());
    findField("tr")->setText(result);
}

void SequencerScreen::displayCount()
{
    findField("count")->setText(sequencer.lock()->isCountEnabled() ? "ON" : "OFF");
}

std::vector<std::string> SequencerScreen::timingCorrectNames = std::vector<std::string>{"OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)"};

void SequencerScreen::displayTiming()
{
    auto noteValue = mpc.screens->get<TimingCorrectScreen>()->getNoteValue();
    findField("timing")->setText(timingCorrectNames[noteValue]);
}

void SequencerScreen::update(Observable *o, Message message)
{
    if (sequence.lock())
    {
        sequence.lock()->deleteObserver(this);

        for (auto &t : sequence.lock()->getTracks())
        {
            t->deleteObserver(this);
        }
    }

    sequence = sequencer.lock()->getActiveSequence();
    sequence.lock()->addObserver(this);

    for (auto &t : sequence.lock()->getTracks())
    {
        t->addObserver(this);
    }

    const auto msg = std::get<std::string>(message);

    if (msg == "nextsqvalue")
    {
        displayNextSq();
    }
    else if (msg == "nextsq")
    {
        displayNextSq();
        ls->setFocus("nextsq");
    }
    else if (msg == "nextsqoff")
    {
        displayNextSq();
    }
    else if (msg == "count")
    {
        displayCount();
    }
    else if (msg == "tracknumbername")
    {
        displayTr();
        displayOn();
    }
    else if (msg == "seqnumbername")
    {
        displaySq();
    }
    else if (msg == "loop")
    {
        displayLoop();
    }
    else if (msg == "recordingmode")
    {
        displayRecordingMode();
    }
    else if (msg == "numberofbars")
    {
        displayBars();
    }
    else if (msg == "trackon")
    {
        displayOn();
    }
    else if (msg == "bar")
    {
        displayNow0();
        displayTempoLabel();
    }
    else if (msg == "beat")
    {
        displayNow1();
        displayTempoLabel();
    }
    else if (msg == "clock")
    {
        displayNow2();
        displayTempoLabel();
    }
    else if (msg == "tempo")
    {
        displayTempo();
    }
    else if (msg == "tempo-source")
    {
        displayTempoSource();
    }
    else if (msg == "timesignature")
    {
        displayTsig();
    }
    else if (msg == "programchange")
    {
        displayPgm();
    }
    else if (msg == "velocityratio")
    {
        displayVelo();
    }
    else if (msg == "bus")
    {
        displayBus();
    }
    else if (msg == "device")
    {
        displayDeviceNumber();
    }
    else if (msg == "devicename")
    {
        displayDeviceName();
    }
}

void SequencerScreen::pressEnter()
{

    auto focusedField = getFocusedFieldOrThrow();

    if (!focusedField->isTypeModeEnabled())
    {
        return;
    }

    auto candidate = focusedField->enter();

    const auto focusedFieldName = focusedField->getName();

    if (candidate != INT_MAX)
    {
        if (focusedFieldName == "now0")
        {
            sequencer.lock()->setBar(candidate - 1);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "now1")
        {
            sequencer.lock()->setBeat(candidate - 1);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "now2")
        {
            sequencer.lock()->setClock(candidate);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "tempo")
        {
            sequencer.lock()->setTempo(candidate * 0.1);
        }
        else if (focusedFieldName == "velo")
        {
            setTrackToUsedIfItIsCurrentlyUnused();
            mpc.getSequencer()->getActiveTrack()->setVelocityRatio(candidate);
        }
    }
}

void SequencerScreen::function(int i)
{
    ScreenComponent::function(i);
    if (sequencer.lock()->isPunchEnabled())
    {
        if (!sequencer.lock()->isRecordingOrOverdubbing() && i == 5)
        {
            sequencer.lock()->setPunchEnabled(false);
            findBackground()->setBackgroundName("sequencer");
            findChild("function-keys")->Hide(false);
        }
        return;
    }

    switch (i)
    {
        case 0:
            if (sequencer.lock()->isPlaying())
            {
                return;
            }

            Util::initSequence(mpc);

            mpc.getLayeredScreen()->openScreen<StepEditorScreen>();
            break;
        case 1:
        {
            if (sequencer.lock()->isPlaying())
            {
                return;
            }

            mpc.getLayeredScreen()->openScreen<EventsScreen>();
            break;
        }
        case 2:
        {
            auto track = mpc.getSequencer()->getActiveTrack();
            track->setOn(!track->isOn());
            break;
        }
        case 3:
        {
            sequencer.lock()->setSoloEnabled(!sequencer.lock()->isSoloEnabled());
            findChild<TextComp>("fk3")->setBlinking(sequencer.lock()->isSoloEnabled());
            break;
        }
        case 4:
            sequencer.lock()->trackDown();
            break;
        case 5:
            sequencer.lock()->trackUp();
            break;
    }
}

void SequencerScreen::setTrackToUsedIfItIsCurrentlyUnused()
{
    auto track = mpc.getSequencer()->getActiveTrack();

    if (!track->isUsed())
    {
        track->setUsed(true);
        displayTr();
    }
}

void SequencerScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.size() >= 3 && focusedFieldName.substr(0, 3) == "now")
    {
        setLastFocus("step-editor", "view");
    }

    if (focusedFieldName.substr(0, 3) == "now" && sequencer.lock()->isPlaying())
    {
        return;
    }

    auto track = mpc.getSequencer()->getActiveTrack();

    if (focusedFieldName == "now0")
    {
        sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() + i);
    }
    else if (focusedFieldName == "now1")
    {
        sequencer.lock()->setBeat(sequencer.lock()->getCurrentBeatIndex() + i);
    }
    else if (focusedFieldName == "now2")
    {
        sequencer.lock()->setClock(sequencer.lock()->getCurrentClockNumber() + i);
    }
    else if (focusedFieldName == "devicenumber")
    {
        setTrackToUsedIfItIsCurrentlyUnused();
        track->setDeviceIndex(track->getDeviceIndex() + i);
    }
    else if (focusedFieldName == "tr")
    {
        if (i > 0)
        {
            sequencer.lock()->trackUp();
        }
        else if (i < 0)
        {
            sequencer.lock()->trackDown();
        }
    }
    else if (focusedFieldName == "bus")
    {
        setTrackToUsedIfItIsCurrentlyUnused();

        track->setBusNumber(track->getBus() + i);

        auto lastFocus = getLastFocus("step-editor");

        if (lastFocus.length() == 2)
        {
            auto eventNumber = stoi(lastFocus.substr(1, 2));

            auto stepEditorScreen = mpc.screens->get<StepEditorScreen>();

            if (std::dynamic_pointer_cast<NoteOnEvent>(stepEditorScreen->getVisibleEvents()[eventNumber]))
            {
                if (track->getBus() == 0)
                {
                    if (lastFocus[0] == 'd' || lastFocus[0] == 'e')
                    {
                        setLastFocus("step-editor", "a" + std::to_string(eventNumber));
                    }
                }
            }
        }
    }
    else if (focusedFieldName == "pgm")
    {
        setTrackToUsedIfItIsCurrentlyUnused();
        track->setProgramChange(track->getProgramChange() + i);
    }
    else if (focusedFieldName == "velo")
    {
        setTrackToUsedIfItIsCurrentlyUnused();
        track->setVelocityRatio(track->getVelocityRatio() + i);
    }
    else if (focusedFieldName == "timing")
    {
        auto screen = mpc.screens->get<TimingCorrectScreen>();
        auto noteValue = screen->getNoteValue();
        screen->setNoteValue(noteValue + i);
        setLastFocus("timing-correct", "notevalue");
        displayTiming();
    }
    else if (focusedFieldName == "sq")
    {
        if (sequencer.lock()->isPlaying())
        {
            if (!sequencer.lock()->isPunchEnabled())
            {
                const auto seqIndex = sequencer.lock()->getCurrentlyPlayingSequenceIndex();

                if (seqIndex + i >= 0)
                {
                    sequencer.lock()->setNextSq(seqIndex + i);
                }
            }
        }
        else
        {
            if (sequencer.lock()->isPunchEnabled())
            {
                sequencer.lock()->setPunchEnabled(false);
                findBackground()->setBackgroundName("sequencer");
                findChild("function-keys")->Hide(false);
            }

            sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
        }
    }
    else if (focusedFieldName == "nextsq")
    {
        if (sequencer.lock()->getNextSq() + i >= 0)
        {
            sequencer.lock()->setNextSq(sequencer.lock()->getNextSq() + i);
        }
    }
    else if (focusedFieldName == "bars")
    {
        if (!sequencer.lock()->isPlaying())
        {
            mpc.getLayeredScreen()->openScreen<ChangeBars2Screen>();
        }
    }
    else if (focusedFieldName == "tempo")
    {
        double oldTempo = sequencer.lock()->getTempo();
        double newTempo = oldTempo + (i * 0.1);
        sequencer.lock()->setTempo(newTempo);
    }
    else if (focusedFieldName == "tsig")
    {
        mpc.getLayeredScreen()->openScreen<ChangeTsigScreen>();
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer.lock()->setTempoSourceSequence(i > 0);
    }
    else if (focusedFieldName == "count")
    {
        sequencer.lock()->setCountEnabled(i > 0);
    }
    else if (focusedFieldName == "loop")
    {
        sequence.lock()->setLoopEnabled(i > 0);
    }
    else if (focusedFieldName == "recordingmode")
    {
        sequencer.lock()->setRecordingModeMulti(i > 0);
    }
    else if (focusedFieldName == "on")
    {
        setTrackToUsedIfItIsCurrentlyUnused();
        track->setOn(i > 0);
    }
}

void SequencerScreen::openWindow()
{
    if (sequencer.lock()->isPlaying())
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sq")
    {
        Util::initSequence(mpc);
        mpc.getLayeredScreen()->openScreen<SequenceScreen>();
    }
    else if (focusedFieldName.find("now") != std::string::npos)
    {
        mpc.getLayeredScreen()->openScreen<TimeDisplayScreen>();
    }
    else if (focusedFieldName.find("tempo") != std::string::npos)
    {
        mpc.getLayeredScreen()->openScreen<TempoChangeScreen>();
    }
    else if (focusedFieldName == "timing")
    {
        mpc.getLayeredScreen()->openScreen<TimingCorrectScreen>();
    }
    else if (focusedFieldName == "tsig")
    {
        mpc.getLayeredScreen()->openScreen<ChangeTsigScreen>();
    }
    else if (focusedFieldName == "count")
    {
        mpc.getLayeredScreen()->openScreen<CountMetronomeScreen>();
    }
    else if (focusedFieldName == "loop")
    {
        mpc.getLayeredScreen()->openScreen<LoopBarsScreen>();
    }
    else if (focusedFieldName == "tr")
    {
        auto track = mpc.getSequencer()->getActiveTrack();

        if (!track->isUsed())
        {
            track->setUsed(true);
        }

        mpc.getLayeredScreen()->openScreen<TrackScreen>();
    }
    else if (focusedFieldName == "on")
    {
        mpc.getLayeredScreen()->openScreen<EraseAllOffTracksScreen>();
    }
    else if (focusedFieldName == "pgm")
    {
        mpc.getLayeredScreen()->openScreen<TransmitProgramChangesScreen>();
    }
    else if (focusedFieldName == "recordingmode")
    {
        mpc.getLayeredScreen()->openScreen<MultiRecordingSetupScreen>();
    }
    else if (focusedFieldName == "bus")
    {
        mpc.getLayeredScreen()->openScreen<MidiInputScreen>();
    }
    else if (focusedFieldName == "devicenumber")
    {
        mpc.getLayeredScreen()->openScreen<MidiOutputScreen>();
    }
    else if (focusedFieldName == "bars")
    {
        mpc.getLayeredScreen()->openScreen<ChangeBarsScreen>();
    }
    else if (focusedFieldName == "velo")
    {
        mpc.getLayeredScreen()->openScreen<EditVelocityScreen>();
    }
}

void SequencerScreen::left()
{
    moveCursor([&]()
               {
                   ScreenComponent::left();
               });
}

void SequencerScreen::right()
{
    moveCursor([&]()
               {
                   ScreenComponent::right();
               });
}

void SequencerScreen::moveCursor(const std::function<void()> &cursorCall)
{
    if (sequencer.lock()->getNextSq() == -1)
    {
        cursorCall();
        return;
    }

    auto defaultTransferMap = getTransferMap();

    for (auto &fieldMap : getTransferMap())
    {
        for (auto &destinationField : fieldMap.second)
        {
            if (destinationField == "sq")
            {
                destinationField = "nextsq";
            }
        }
    }

    getTransferMap()["nextsq"] = defaultTransferMap["sq"];
    getTransferMap()["now0"] = {"nextsq", "_", "_", "tsig"};

    cursorCall();
    setTransferMap(defaultTransferMap);
}

void SequencerScreen::up()
{
    moveCursor([&]()
               {
                   ScreenComponent::up();
               });
}

void SequencerScreen::down()
{
    moveCursor([&]()
               {
                   ScreenComponent::down();
               });
}

void SequencerScreen::setPunchRectOn(int i, bool b)
{
    findChild<PunchRect>("punch-rect-" + std::to_string(i))->setOn(b);
}

void SequencerScreen::displayPunchWhileRecording()
{
    auto punchScreen = mpc.screens->get<PunchScreen>();

    auto hardware = mpc.getHardware();
    auto isRecPressedOrLocked = hardware->getButton(hardware::ComponentId::REC)->isPressed() || mpc.clientEventController->clientHardwareEventController->buttonLockTracker.isLocked(hardware::ComponentId::REC);
    auto isOverdubPressedOrLocked = hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() || mpc.clientEventController->clientHardwareEventController->buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);

    if (sequencer.lock()->isPunchEnabled() && (isRecPressedOrLocked || isOverdubPressedOrLocked))
    {
        findBackground()->setBackgroundName("sequencer");

        for (int i = 0; i < 3; i++)
        {
            auto punchRect = findChild<PunchRect>("punch-rect-" + std::to_string(i));
            punchRect->Hide((i == 0 && sequencer.lock()->getAutoPunchMode() == 1) || (i == 2 && sequencer.lock()->getAutoPunchMode() == 0));
            punchRect->setOn((i == 0 && sequencer.lock()->getAutoPunchMode() != 1) || (i == 1 && sequencer.lock()->getAutoPunchMode() == 1));
        }

        auto time0 = findLabel("punch-time-0");
        auto time1 = findLabel("punch-time-1");

        time0->Hide(sequencer.lock()->getAutoPunchMode() == 1);
        time1->Hide(sequencer.lock()->getAutoPunchMode() == 0);

        auto seq = sequence.lock();

        auto text1 = StrUtil::padLeft(std::to_string(SeqUtil::getBar(seq.get(), sequencer.lock()->getPunchInTime()) + 1), "0", 3);
        auto text2 = StrUtil::padLeft(std::to_string(SeqUtil::getBeat(seq.get(), sequencer.lock()->getPunchInTime()) + 1), "0", 2);
        auto text3 = StrUtil::padLeft(std::to_string(SeqUtil::getClock(seq.get(), sequencer.lock()->getPunchInTime())), "0", 2);
        auto text4 = StrUtil::padLeft(std::to_string(SeqUtil::getBar(seq.get(), sequencer.lock()->getPunchOutTime()) + 1), "0", 3);
        auto text5 = StrUtil::padLeft(std::to_string(SeqUtil::getBeat(seq.get(), sequencer.lock()->getPunchOutTime()) + 1), "0", 2);
        auto text6 = StrUtil::padLeft(std::to_string(SeqUtil::getClock(seq.get(), sequencer.lock()->getPunchOutTime())), "0", 2);

        time0->setText("IN:" + text1 + "." + text2 + "." + text3);
        time1->setText("OUT:" + text4 + "." + text5 + "." + text6);
    }
}

void SequencerScreen::displayNextSq()
{
    ls->setFunctionKeysArrangement(sequencer.lock()->getNextSq() == -1 ? 0 : 1);

    auto noNextSq = sequencer.lock()->getNextSq() == -1;
    findLabel("nextsq")->Hide(noNextSq);
    findField("nextsq")->Hide(noNextSq);

    if (noNextSq)
    {
        return;
    }

    findField("nextsq")->setTextPadded(sequencer.lock()->getNextSq() + 1);
}

void SequencerScreen::play()
{
    ScreenComponent::play();
    displayPunchWhileRecording();
}

void SequencerScreen::playStart()
{
    ScreenComponent::playStart();
    displayPunchWhileRecording();
}

void SequencerScreen::stop()
{
    ScreenComponent::stop();

    if (sequencer.lock()->isPunchEnabled())
    {
        findBackground()->setBackgroundName("sequencer-punch-active");

        for (int i = 0; i < 3; i++)
        {
            findChild<PunchRect>("punch-rect-" + std::to_string(i))->Hide(true);
        }

        auto time0 = findLabel("punch-time-0");
        auto time1 = findLabel("punch-time-1");

        time0->Hide(true);
        time1->Hide(true);
    }
}

void SequencerScreen::rec()
{
    Util::initSequence(mpc);
    ScreenComponent::rec();
}

void SequencerScreen::overDub()
{
    Util::initSequence(mpc);
    ScreenComponent::overDub();
}
