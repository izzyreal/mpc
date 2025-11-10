#include "TrMuteScreen.hpp"
#include "sequencer/Transport.hpp"

#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Label.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;

TrMuteScreen::TrMuteScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "track-mute", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer->isSoloEnabled();
                        },
                        [&](auto)
                        {
                            refreshTracks();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->isSoloEnabled();
                        },
                        [&](auto)
                        {
                            refreshTracks();
                        }});

    addReactiveBinding({[&]
                        {
                            return mpc.clientEventController->getActiveBank();
                        },
                        [&](auto)
                        {
                            displayBank();
                            displayTrackNumbers();
                            refreshTracks();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTransport()->getTickPosition();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getActiveSequence();
                        },
                        [&](auto)
                        {
                            displaySq();
                            refreshTracks();
                        }});

    addReactiveBinding({[&]
                        {
                            const int bank = static_cast<int>(
                                mpc.clientEventController->getActiveBank());

                            uint16_t tracksEnabled = 0;

                            for (int i = 0; i < 16; ++i)
                            {
                                const int trackIndex = bank * 16 + i;
                                if (sequencer->getActiveSequence()
                                        ->getTrack(trackIndex)
                                        ->isOn())
                                {
                                    tracksEnabled |= (1u << i);
                                }
                            }

                            return tracksEnabled;
                        },
                        [&](auto)
                        {
                            refreshTracks();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getActiveTrackIndex();
                        },
                        [&](auto)
                        {
                            refreshTracks();
                        }});
}

void TrMuteScreen::open()
{
    if (sequencer->isSoloEnabled())
    {
        findBackground()->setBackgroundName("track-mute-solo-2");
    }
    else
    {
        findBackground()->setBackgroundName("track-mute");
    }

    for (int i = 0; i < 16; i++)
    {
        const auto trackField = findField(std::to_string(i + 1));
        trackField->setSize(49, 9);
        trackField->setFocusable(false);
    }

    displayBank();
    displayTrackNumbers();

    for (int i = 0; i < 16; i++)
    {
        displayTrack(i);
        setTrackColor(i);
    }

    displaySq();
    displayNow0();
    displayNow1();
    displayNow2();
}

void TrMuteScreen::right()
{
    // Stop right from propgating to BaseController
}

void TrMuteScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sq" && !sequencer->getTransport()->isPlaying())
    {
        sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() +
                                          i);
    }
}

void TrMuteScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 5:
            if (sequencer->isSoloEnabled())
            {
                ls->setCurrentBackground("track-mute");
                sequencer->setSoloEnabled(false);
            }
            else
            {
                ls->setCurrentBackground("track-mute-solo-1");
            }
            break;
    }
}

int TrMuteScreen::bankoffset() const
{
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    return bank * 16;
}

void TrMuteScreen::displayBank() const
{
    const std::vector<std::string> letters{"A", "B", "C", "D"};
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    findLabel("bank")->setText(letters[bank]);
}

void TrMuteScreen::displayTrackNumbers() const
{
    const std::vector<std::string> trn{"01-16", "17-32", "33-48", "49-64"};
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    findLabel("tracknumbers")->setText(trn[bank]);
}

void TrMuteScreen::displaySq() const
{
    const auto sequenceNumber = StrUtil::padLeft(
        std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2);
    const auto sequenceName = sequencer->getActiveSequence()->getName();
    findField("sq")->setText(sequenceNumber + "-" + sequenceName);
}

void TrMuteScreen::displayTrack(int i) const
{
    findField(std::to_string(i + 1))
        ->setText(sequencer->getActiveSequence()
                      ->getTrack(i + bankoffset())
                      ->getName()
                      .substr(0, 8));
}

void TrMuteScreen::setTrackColor(int i) const
{
    if (sequencer->isSoloEnabled())
    {
        findField(std::to_string(i + 1))
            ->setInverted(i + bankoffset() == sequencer->getActiveTrackIndex());
    }
    else
    {
        findField(std::to_string(i + 1))
            ->setInverted(sequencer->getActiveSequence()
                              ->getTrack(i + bankoffset())
                              ->isOn());
    }
}

void TrMuteScreen::displayNow0() const
{
    findField("now0")->setTextPadded(sequencer->getTransport()->getCurrentBarIndex() + 1, "0");
}

void TrMuteScreen::displayNow1() const
{
    findField("now1")->setTextPadded(sequencer->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void TrMuteScreen::displayNow2() const
{
    findField("now2")->setTextPadded(sequencer->getTransport()->getCurrentClockNumber(), "0");
}

void TrMuteScreen::refreshTracks() const
{
    for (int i = 0; i < 16; i++)
    {
        displayTrack(i);
        setTrackColor(i);
    }
}
