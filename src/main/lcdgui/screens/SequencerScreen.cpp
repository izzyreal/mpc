#include "SequencerScreen.hpp"
#include "sequencer/Transport.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/TimeSignature.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/PunchRect.hpp"

#include "sequencer/SeqUtil.hpp"
#include "Util.hpp"
#include "Mpc.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

SequencerScreen::SequencerScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sequencer", layerIndex)
{
    MRECT punch0(0, 52, 30, 59);
    addChildT<PunchRect>("punch-rect-0", punch0)->Hide(true);

    MRECT punch1(105, 52, 135, 59);
    addChildT<PunchRect>("punch-rect-1", punch1)->Hide(true);

    MRECT punch2(217, 52, 247, 59);
    addChildT<PunchRect>("punch-rect-2", punch2)->Hide(true);

    addChildT<TextComp>(mpc, "footer-label");
    const auto footerLabel = findChild<TextComp>("footer-label");
    footerLabel->setLocation(36, 51);
    footerLabel->setText("(Hold pads or keys to repeat)");
    footerLabel->setSize(footerLabel->GetTextEntryLength() * 6, 8);
    footerLabel->Hide(true);

    auto displaySequenceProps = [&]
    {
        displaySq();
        displayTsig();
        displayBars();
        displayTempo();
        displayLoop();
    };

    auto displayTrackProps = [&]
    {
        displayTr();
        displayOn();
        displayPgm();
        displayVelo();
        displayBus();
        displayDeviceNumber();
        displayDeviceName();
    };

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSequenceIndex();
                        },
                        [displaySequenceProps, displayTrackProps](auto)
                        {
                            displaySequenceProps();
                            displayTrackProps();
                        }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedSequence()->isUsed();
         },
         [displaySequenceProps, displayTrackProps](auto)
         {
             displaySequenceProps();
             displayTrackProps();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getTransport()->isCountEnabled();
         },
         [&](auto)
         {
             displayCount();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedSequence()->isLoopEnabled();
         },
         [&](auto)
         {
             displayLoop();
         }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->isRecordingModeMulti();
                        },
                        [&](auto)
                        {
                            displayRecordingMode();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedTrack()->isOn();
                        },
                        [&](auto)
                        {
                            displayOn();
                        }});

    addReactiveBinding(
        {[&]
         {
             if (sequencer.lock()->getTransport()->isCountingIn())
             {
                 return static_cast<int>(Sequencer::quarterNotesToTicks(
                     sequencer.lock()
                         ->getTransport()
                         ->getPlayStartPositionQuarterNotes()));
             }
             return sequencer.lock()->getTransport()->getTickPosition();
         },
         [&](auto)
         {
             displayNow0();
             displayNow1();
             displayNow2();
         }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getTransport()->getTempo();
                        },
                        [&](auto)
                        {
                            displayTempo();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getTransport()
                                ->isTempoSourceSequenceEnabled();
                        },
                        [&](auto)
                        {
                            displayTempoSource();
                        }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedTrack()->getProgramChange();
         },
         [&](auto)
         {
             displayPgm();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedSequence()->getTimeSignature();
         },
         [&](auto)
         {
             displayTsig();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedTrack()->getVelocityRatio();
         },
         [&](auto)
         {
             displayVelo();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedTrack()->getBusType();
         },
         [&](auto)
         {
             displayBus();
         }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedTrack()->getDeviceIndex();
         },
         [&](auto)
         {
             displayDeviceNumber();
             displayDeviceName();
         }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedTrackIndex();
                        },
                        [displayTrackProps](auto)
                        {
                            displayTrackProps();
                        }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedTrack()->isUsed();
         },
         [displayTrackProps](auto)
         {
             displayTrackProps();
         }});

    addReactiveBinding({[&]
                        {
                            return mpc.screens
                                ->get<ScreenId::TimingCorrectScreen>()
                                ->getNoteValue();
                        },
                        [&](auto)
                        {
                            displayTiming();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getNextSq();
                        },
                        [&](auto nextSqIndex)
                        {
                            displayNextSq();
                            if (nextSqIndex != -1)
                            {
                                ls.lock()->setFocus("nextsq");
                            }
                        }});
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

    findChild<TextComp>("fk3")->setBlinking(sequencer.lock()->isSoloEnabled());

    if (sequencer.lock()->isSecondSequenceEnabled())
    {
        findBackground()->setBackgroundName("sequencer-2nd");
    }
    else if (sequencer.lock()->getTransport()->isPunchEnabled() &&
             !sequencer.lock()->getTransport()->isRecordingOrOverdubbing())
    {
        findBackground()->setBackgroundName("sequencer-punch-active");
    }
    else
    {
        findBackground()->setBackgroundName("sequencer");
    }

    if (sequencer.lock()->getNextSq() != -1)
    {
        ls.lock()->setFocus("nextsq");
    }

    const bool sequencerIsRecordingOrOverdubbing =
        sequencer.lock()->getTransport()->isRecordingOrOverdubbing();

    const auto footerIsInvisible =
        !mpc.clientEventController->clientHardwareEventController
             ->isNoteRepeatLocked() &&
        !(mpc.getHardware()
              ->getButton(hardware::ComponentId::ERASE)
              ->isPressed() &&
          sequencerIsRecordingOrOverdubbing);

    findChild("footer-label")->Hide(footerIsInvisible);

    findChild("function-keys")
        ->Hide(!footerIsInvisible ||
               sequencer.lock()->getTransport()->isPunchEnabled() ||
               (mpc.getHardware()
                    ->getButton(hardware::ComponentId::ERASE)
                    ->isPressed() &&
                sequencerIsRecordingOrOverdubbing));
}

void SequencerScreen::erase()
{
    findChild("function-keys")->Hide(true);
    findChild("footer-label")->Hide(false);
    findChild<TextComp>("footer-label")
        ->setText("(Hold pads or keys to erase)");
}

void SequencerScreen::tap()
{
    if (sequencer.lock()->getTransport()->isPlaying())
    {
        if (mpc.clientEventController->clientHardwareEventController
                ->isNoteRepeatLocked())
        {
            findChild("function-keys")->Hide(false);
            findChild("footer-label")->Hide(true);
        }
        else
        {
            findChild("function-keys")->Hide(true);
            findChild("footer-label")->Hide(false);
            findChild<TextComp>("footer-label")
                ->setText("(Hold pads or keys to repeat)");
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
    std::vector<std::string> screensThatDisablePunch{"song", "load", "save",
                                                     "others", "next-seq"};

    if (const auto nextScreen = ls.lock()->getCurrentScreenName();
        find(begin(screensThatDisablePunch), end(screensThatDisablePunch),
             nextScreen) != end(screensThatDisablePunch))
    {
        sequencer.lock()->getTransport()->setPunchEnabled(false);
    }

    sequencer.lock()->resetUndo();
}

void SequencerScreen::displayVelo() const
{
    findField("velo")->setTextPadded(std::to_string(
        sequencer.lock()->getSelectedTrack()->getVelocityRatio()));
}

void SequencerScreen::displayDeviceNumber() const
{
    if (const auto track = mpc.getSequencer()->getSelectedTrack();
        track->getDeviceIndex() == 0)
    {
        findField("devicenumber")->setText("OFF");
    }
    else
    {
        if (track->getDeviceIndex() >= 17)
        {
            findField("devicenumber")
                ->setText(std::to_string(track->getDeviceIndex() - 16) + "B");
        }
        else
        {
            findField("devicenumber")
                ->setText(std::to_string(track->getDeviceIndex()) + "A");
        }
    }
}

void SequencerScreen::displayBus() const
{
    findField("bus")->setText(
        busTypeToString(sequencer.lock()->getSelectedTrack()->getBusType()));
    displayDeviceName();
}

void SequencerScreen::displayBars() const
{
    findField("bars")->setText(std::to_string(
        sequencer.lock()->getSelectedSequence()->getLastBarIndex() + 1));
}

void SequencerScreen::displayPgm() const
{
    if (const auto track = mpc.getSequencer()->getSelectedTrack();
        track->getProgramChange() == 0)
    {
        findField("pgm")->setText("OFF");
    }
    else
    {
        findField("pgm")->setText(std::to_string(track->getProgramChange()));
    }
}

void SequencerScreen::displayDeviceName() const
{
    const auto track = mpc.getSequencer()->getSelectedTrack();

    if (const auto drumBus =
            sequencer.lock()->getBus<DrumBus>(track->getBusType());
        drumBus)
    {
        if (track->getDeviceIndex() == 0)
        {
            const int pgm = drumBus->getProgramIndex();
            const auto p = sampler.lock()->getProgram(pgm);
            findLabel("devicename")->setText(p->getName());
        }
        else
        {
            findLabel("devicename")
                ->setText(
                    sequencer.lock()->getSelectedSequence()->getDeviceName(
                        track->getDeviceIndex()));
        }
    }
    else
    {
        if (track->getDeviceIndex() == 0)
        {
            findLabel("devicename")->setText("NewPgm-A");
        }
        else
        {
            findLabel("devicename")
                ->setText(
                    sequencer.lock()->getSelectedSequence()->getDeviceName(
                        track->getDeviceIndex()));
        }
    }
}

void SequencerScreen::displayTempo() const
{
    displayTempoLabel();
    findField("tempo")->setText(StrUtil::padLeft(
        Util::tempoString(sequencer.lock()->getTransport()->getTempo()), " ",
        6));
}

void SequencerScreen::displayTempoLabel() const
{
    auto currentRatio = -1;
    const auto seq = sequencer.lock()->getSelectedSequence();

    if (!seq->isUsed() || !seq->isTempoChangeOn())
    {
        findLabel("tempo")->setText(u8" \u00C0:");
        return;
    }

    for (const auto &tce : seq->getTempoChangeEvents())
    {
        if (tce->getTick() >
            sequencer.lock()->getTransport()->getTickPosition())
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

void SequencerScreen::displayTempoSource() const
{
    findField("tempo-source")
        ->setText(
            sequencer.lock()->getTransport()->isTempoSourceSequenceEnabled()
                ? "(SEQ)"
                : "(MAS)");
}

void SequencerScreen::displaySq() const
{
    std::string result;

    if (sequencer.lock()->getTransport()->isPlaying())
    {
        result.append(StrUtil::padLeft(
            std::to_string(
                sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1),
            "0", 2));
        result.append("-");
        result.append(
            sequencer.lock()->getCurrentlyPlayingSequence()->getName());
        findField("sq")->setText(result);
    }
    else
    {
        result.append(StrUtil::padLeft(
            std::to_string(sequencer.lock()->getSelectedSequenceIndex() + 1),
            "0", 2));
        result.append("-");
        result.append(sequencer.lock()->getSelectedSequence()->getName());
        findField("sq")->setText(result);
    }
}

void SequencerScreen::displayNow0() const
{
    findField("now0")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBarIndex() + 1, "0");
}

void SequencerScreen::displayNow1() const
{
    findField("now1")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void SequencerScreen::displayNow2() const
{
    findField("now2")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentClockNumber(), "0");
}

void SequencerScreen::displayRecordingMode() const
{
    findField("recordingmode")
        ->setText(sequencer.lock()->isRecordingModeMulti() ? "M" : "S");
}

void SequencerScreen::displayTsig() const
{
    std::string result;
    const auto ts = sequencer.lock()->getSelectedSequence()->getTimeSignature();
    result.append(std::to_string(ts.numerator));
    result.append("/");
    result.append(std::to_string(ts.denominator));
    findField("tsig")->setText(Util::distributeTimeSig(result));
}

void SequencerScreen::displayLoop() const
{
    findField("loop")->setText(
        sequencer.lock()->getSelectedSequence()->isLoopEnabled() ? "ON"
                                                                 : "OFF");
}

void SequencerScreen::displayOn() const
{
    findField("on")->setText(
        sequencer.lock()->getSelectedTrack()->isOn() ? "YES" : "NO");
}

void SequencerScreen::displayTr() const
{

    auto result = StrUtil::padLeft(
        std::to_string(sequencer.lock()->getSelectedTrackIndex() + 1), "0", 2);
    result.append("-");
    result.append(sequencer.lock()->getSelectedTrack()->getName());
    findField("tr")->setText(result);
}

void SequencerScreen::displayCount() const
{
    findField("count")->setText(
        sequencer.lock()->getTransport()->isCountEnabled() ? "ON" : "OFF");
}

std::vector<std::string> SequencerScreen::timingCorrectNames =
    std::vector<std::string>{"OFF",     "1/8",  "1/8(3)", "1/16",
                             "1/16(3)", "1/32", "1/32(3)"};

void SequencerScreen::displayTiming() const
{
    const auto noteValue =
        mpc.screens->get<ScreenId::TimingCorrectScreen>()->getNoteValue();
    findField("timing")->setText(timingCorrectNames[noteValue]);
}

void SequencerScreen::pressEnter()
{
    const auto focusedField = getFocusedFieldOrThrow();

    if (!focusedField->isTypeModeEnabled())
    {
        return;
    }

    const auto candidate = focusedField->enter();

    const auto focusedFieldName = focusedField->getName();

    if (candidate != INT_MAX)
    {
        if (focusedFieldName == "now0")
        {
            sequencer.lock()->getTransport()->setBar(candidate - 1);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "now1")
        {
            sequencer.lock()->getTransport()->setBeat(candidate - 1);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "now2")
        {
            sequencer.lock()->getTransport()->setClock(candidate);
            setLastFocus("step-editor", "view");
        }
        else if (focusedFieldName == "tempo")
        {
            sequencer.lock()->getTransport()->setTempo(candidate * 0.1);
        }
        else if (focusedFieldName == "velo")
        {
            setTrackToUsedIfItIsCurrentlyUnused();
            mpc.getSequencer()->getSelectedTrack()->setVelocityRatio(candidate);
        }
    }
}

void SequencerScreen::function(const int i)
{
    ScreenComponent::function(i);
    if (sequencer.lock()->getTransport()->isPunchEnabled())
    {
        if (!sequencer.lock()->getTransport()->isRecordingOrOverdubbing() &&
            i == 5)
        {
            sequencer.lock()->getTransport()->setPunchEnabled(false);
            findBackground()->setBackgroundName("sequencer");
            findChild("function-keys")->Hide(false);
        }
        return;
    }

    switch (i)
    {
        case 0:
            if (sequencer.lock()->getTransport()->isPlaying())
            {
                return;
            }

            Util::initSequence(mpc);

            openScreenById(ScreenId::StepEditorScreen);
            break;
        case 1:
        {
            if (sequencer.lock()->getTransport()->isPlaying())
            {
                return;
            }

            openScreenById(ScreenId::EventsScreen);
            break;
        }
        case 2:
        {
            const auto track = mpc.getSequencer()->getSelectedTrack();
            track->setOn(!track->isOn());
            break;
        }
        case 3:
        {
            sequencer.lock()->setSoloEnabled(
                !sequencer.lock()->isSoloEnabled());
            findChild<TextComp>("fk3")->setBlinking(
                sequencer.lock()->isSoloEnabled());
            break;
        }
        case 4:
            sequencer.lock()->trackDown();
            break;
        case 5:
            sequencer.lock()->trackUp();
            break;
        default:;
    }
}

void SequencerScreen::setTrackToUsedIfItIsCurrentlyUnused() const
{
    if (const auto track = mpc.getSequencer()->getSelectedTrack();
        !track->isUsed())
    {
        track->setUsed(true);
        displayTr();
    }
}

void SequencerScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.size() >= 3 && focusedFieldName.substr(0, 3) == "now")
    {
        setLastFocus("step-editor", "view");
    }

    if (focusedFieldName.substr(0, 3) == "now" &&
        sequencer.lock()->getTransport()->isPlaying())
    {
        return;
    }

    const auto track = mpc.getSequencer()->getSelectedTrack();

    if (focusedFieldName == "now0")
    {
        sequencer.lock()->getTransport()->setBar(
            sequencer.lock()->getTransport()->getCurrentBarIndex() + i);
    }
    else if (focusedFieldName == "now1")
    {
        sequencer.lock()->getTransport()->setBeat(
            sequencer.lock()->getTransport()->getCurrentBeatIndex() + i);
    }
    else if (focusedFieldName == "now2")
    {
        sequencer.lock()->getTransport()->setClock(
            sequencer.lock()->getTransport()->getCurrentClockNumber() + i);
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

        track->setBusType(track->getBusType() + i);

        if (const auto lastFocus = getLastFocus("step-editor");
            lastFocus.length() == 2)
        {
            const auto eventNumber = stoi(lastFocus.substr(1, 2));

            const auto stepEditorScreen =
                mpc.screens->get<ScreenId::StepEditorScreen>();

            if (std::dynamic_pointer_cast<NoteOnEvent>(
                    stepEditorScreen->computeVisibleEvents()[eventNumber]))
            {
                if (isDrumBusType(track->getBusType()))
                {
                    if (lastFocus[0] == 'd' || lastFocus[0] == 'e')
                    {
                        setLastFocus("step-editor",
                                     "a" + std::to_string(eventNumber));
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
        const auto screen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
        const auto noteValue = screen->getNoteValue();
        screen->setNoteValue(noteValue + i);
        setLastFocus("timing-correct", "notevalue");
    }
    else if (focusedFieldName == "sq")
    {
        if (sequencer.lock()->getTransport()->isPlaying())
        {
            if (!sequencer.lock()->getTransport()->isPunchEnabled())
            {
                const auto seqIndex =
                    sequencer.lock()->getCurrentlyPlayingSequenceIndex();

                if (seqIndex + i >= 0)
                {
                    sequencer.lock()->setNextSq(seqIndex + i);
                }
            }
        }
        else
        {
            if (sequencer.lock()->getTransport()->isPunchEnabled())
            {
                sequencer.lock()->getTransport()->setPunchEnabled(false);
                findBackground()->setBackgroundName("sequencer");
                findChild("function-keys")->Hide(false);
            }

            sequencer.lock()->setSelectedSequenceIndex(
                sequencer.lock()->getSelectedSequenceIndex() + i, true);
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
        if (!sequencer.lock()->getTransport()->isPlaying())
        {
            openScreenById(ScreenId::ChangeBars2Screen);
        }
    }
    else if (focusedFieldName == "tempo")
    {
        const double oldTempo = sequencer.lock()->getTransport()->getTempo();
        const double newTempo = oldTempo + i * 0.1;
        sequencer.lock()->getTransport()->setTempo(newTempo);
    }
    else if (focusedFieldName == "tsig")
    {
        openScreenById(ScreenId::ChangeTsigScreen);
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer.lock()->getTransport()->setTempoSourceSequence(i > 0);
    }
    else if (focusedFieldName == "count")
    {
        sequencer.lock()->getTransport()->setCountEnabled(i > 0);
    }
    else if (focusedFieldName == "loop")
    {
        sequencer.lock()->getSelectedSequence()->setLoopEnabled(i > 0);
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
    if (sequencer.lock()->getTransport()->isPlaying())
    {
        return;
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sq")
    {
        Util::initSequence(mpc);
        openScreenById(ScreenId::SequenceScreen);
    }
    else if (focusedFieldName.find("now") != std::string::npos)
    {
        openScreenById(ScreenId::TimeDisplayScreen);
    }
    else if (focusedFieldName.find("tempo") != std::string::npos)
    {
        openScreenById(ScreenId::TempoChangeScreen);
    }
    else if (focusedFieldName == "timing")
    {
        openScreenById(ScreenId::TimingCorrectScreen);
    }
    else if (focusedFieldName == "tsig")
    {
        openScreenById(ScreenId::ChangeTsigScreen);
    }
    else if (focusedFieldName == "count")
    {
        openScreenById(ScreenId::CountMetronomeScreen);
    }
    else if (focusedFieldName == "loop")
    {
        openScreenById(ScreenId::LoopBarsScreen);
    }
    else if (focusedFieldName == "tr")
    {
        if (const auto track = mpc.getSequencer()->getSelectedTrack();
            !track->isUsed())
        {
            track->setUsed(true);
        }

        openScreenById(ScreenId::TrackScreen);
    }
    else if (focusedFieldName == "on")
    {
        openScreenById(ScreenId::EraseAllOffTracksScreen);
    }
    else if (focusedFieldName == "pgm")
    {
        openScreenById(ScreenId::TransmitProgramChangesScreen);
    }
    else if (focusedFieldName == "recordingmode")
    {
        openScreenById(ScreenId::MultiRecordingSetupScreen);
    }
    else if (focusedFieldName == "bus")
    {
        openScreenById(ScreenId::MidiInputScreen);
    }
    else if (focusedFieldName == "devicenumber")
    {
        openScreenById(ScreenId::MidiOutputScreen);
    }
    else if (focusedFieldName == "bars")
    {
        openScreenById(ScreenId::ChangeBarsScreen);
    }
    else if (focusedFieldName == "velo")
    {
        openScreenById(ScreenId::EditVelocityScreen);
    }
}

void SequencerScreen::left()
{
    moveCursor(
        [&]
        {
            ScreenComponent::left();
        });
}

void SequencerScreen::right()
{
    moveCursor(
        [&]
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

    for (auto &[first, second] : getTransferMap())
    {
        for (auto &destinationField : second)
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
    moveCursor(
        [&]
        {
            ScreenComponent::up();
        });
}

void SequencerScreen::down()
{
    moveCursor(
        [&]
        {
            ScreenComponent::down();
        });
}

void SequencerScreen::setPunchRectOn(const int i, const bool b)
{
    findChild<PunchRect>("punch-rect-" + std::to_string(i))->setOn(b);
}

void SequencerScreen::displayPunchWhileRecording()
{
    auto hardware = mpc.getHardware();
    auto isRecPressedOrLocked =
        hardware->getButton(hardware::ComponentId::REC)->isPressed() ||
        mpc.clientEventController->clientHardwareEventController
            ->buttonLockTracker.isLocked(hardware::ComponentId::REC);
    auto isOverdubPressedOrLocked =
        hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() ||
        mpc.clientEventController->clientHardwareEventController
            ->buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);

    if (sequencer.lock()->getTransport()->isPunchEnabled() &&
        (isRecPressedOrLocked || isOverdubPressedOrLocked))
    {
        findBackground()->setBackgroundName("sequencer");

        for (int i = 0; i < 3; i++)
        {
            auto punchRect =
                findChild<PunchRect>("punch-rect-" + std::to_string(i));
            punchRect->Hide(
                (i == 0 &&
                 sequencer.lock()->getTransport()->getAutoPunchMode() == 1) ||
                (i == 2 &&
                 sequencer.lock()->getTransport()->getAutoPunchMode() == 0));
            punchRect->setOn(
                (i == 0 &&
                 sequencer.lock()->getTransport()->getAutoPunchMode() != 1) ||
                (i == 1 &&
                 sequencer.lock()->getTransport()->getAutoPunchMode() == 1));
        }

        auto time0 = findLabel("punch-time-0");
        auto time1 = findLabel("punch-time-1");

        time0->Hide(sequencer.lock()->getTransport()->getAutoPunchMode() == 1);
        time1->Hide(sequencer.lock()->getTransport()->getAutoPunchMode() == 0);

        auto seq = sequencer.lock()->getSelectedSequence();

        auto text1 = StrUtil::padLeft(
            std::to_string(
                SeqUtil::getBar(
                    seq.get(),
                    sequencer.lock()->getTransport()->getPunchInTime()) +
                1),
            "0", 3);
        auto text2 = StrUtil::padLeft(
            std::to_string(
                SeqUtil::getBeat(
                    seq.get(),
                    sequencer.lock()->getTransport()->getPunchInTime()) +
                1),
            "0", 2);
        auto text3 = StrUtil::padLeft(
            std::to_string(SeqUtil::getClock(
                seq.get(), sequencer.lock()->getTransport()->getPunchInTime())),
            "0", 2);
        auto text4 = StrUtil::padLeft(
            std::to_string(
                SeqUtil::getBar(
                    seq.get(),
                    sequencer.lock()->getTransport()->getPunchOutTime()) +
                1),
            "0", 3);
        auto text5 = StrUtil::padLeft(
            std::to_string(
                SeqUtil::getBeat(
                    seq.get(),
                    sequencer.lock()->getTransport()->getPunchOutTime()) +
                1),
            "0", 2);
        auto text6 = StrUtil::padLeft(
            std::to_string(SeqUtil::getClock(
                seq.get(),
                sequencer.lock()->getTransport()->getPunchOutTime())),
            "0", 2);

        time0->setText("IN:" + text1 + "." + text2 + "." + text3);
        time1->setText("OUT:" + text4 + "." + text5 + "." + text6);
    }
}

void SequencerScreen::displayNextSq() const
{
    ls.lock()->setFunctionKeysArrangement(
        sequencer.lock()->getNextSq() == -1 ? 0 : 1);

    const auto noNextSq = sequencer.lock()->getNextSq() == -1;
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

    if (sequencer.lock()->getTransport()->isPunchEnabled())
    {
        findBackground()->setBackgroundName("sequencer-punch-active");

        for (int i = 0; i < 3; i++)
        {
            findChild<PunchRect>("punch-rect-" + std::to_string(i))->Hide(true);
        }

        const auto time0 = findLabel("punch-time-0");
        const auto time1 = findLabel("punch-time-1");

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
