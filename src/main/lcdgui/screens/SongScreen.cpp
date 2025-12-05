#include "SongScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Song.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens;

SongScreen::SongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "song", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getTransport()
                                ->getTickPositionGuiPresentation();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSongStepIndex();
                        },
                        [&](auto)
                        {
                            displaySteps();
                            displayTempo();
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSongIndex();
                        },
                        [&](auto)
                        {
                            displaySongName();
                            displayLoop();
                            displaySteps();
                            displayTempo();
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSongSequenceIndex();
                        },
                        [&](auto)
                        {
                            displaySteps();
                            displayTempo();
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

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getTransport()->isPlaying();
         },
         [&](auto isPlaying)
         {
             findField("sequence1")->setBlinking(isPlaying, 10);
             findField("reps1")->setBlinking(isPlaying, 10);
         }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getTransport()
                                ->getPlayedStepRepetitions();
                        },
                        [&](auto)
                        {
                            displaySteps();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSequenceIndex();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});
}

void SongScreen::open()
{
    findField("loop")->setAlignment(Alignment::Centered);

    for (int i = 0; i < 3; i++)
    {
        findField("step" + std::to_string(i))
            ->setAlignment(Alignment::Centered);
        findField("reps" + std::to_string(i))
            ->setAlignment(Alignment::Centered);
    }

    displaySongName();
    displayNow0();
    displayNow1();
    displayNow2();
    displayTempoSource();
    displayTempo();
    displayLoop();
    displaySteps();
}

void SongScreen::up()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "step1" || focusedFieldName == "sequence1" ||
        focusedFieldName == "reps1")
    {
        const auto lockedSequencer = sequencer.lock();

        if (lockedSequencer->getTransport()->isPlaying())
        {
            return;
        }

        const auto currentStepIndex =
            lockedSequencer->getSelectedSongStepIndex();

        lockedSequencer->setSelectedSongStepIndex(currentStepIndex - 1);
    }
    else
    {
        ScreenComponent::up();
    }
}

void SongScreen::down()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "step1" || focusedFieldName == "sequence1" ||
        focusedFieldName == "reps1")
    {
        const auto lockedSequencer = sequencer.lock();

        if (lockedSequencer->getTransport()->isPlaying())
        {
            return;
        }

        const auto currentStepIndex =
            lockedSequencer->getSelectedSongStepIndex();

        lockedSequencer->setSelectedSongStepIndex(currentStepIndex + 1);
    }
    else
    {
        ScreenComponent::down();
    }
}

void SongScreen::left()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sequence1")
    {
        ls.lock()->setFocus("step1");
    }
    else if (focusedFieldName == "reps1")
    {
        ls.lock()->setFocus("sequence1");
    }
    else if (focusedFieldName == "step1")
    {
        ls.lock()->setFocus("loop");
    }
    else
    {
        ScreenComponent::left();
    }
}

void SongScreen::right()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sequence1")
    {
        ls.lock()->setFocus("reps1");
    }
    else if (focusedFieldName == "step1")
    {
        ls.lock()->setFocus("sequence1");
    }
    else if (focusedFieldName == "loop")
    {
        ls.lock()->setFocus("step1");
    }
    else
    {
        ScreenComponent::right();
    }
}

void SongScreen::openWindow()
{
    const auto lockedSequencer = sequencer.lock();

    if (lockedSequencer->getTransport()->isPlaying())
    {
        return;
    }

    const auto song = lockedSequencer->getSelectedSong();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("now") == std::string::npos && !song->isUsed())
    {
        song->setUsed(true);

        const auto songName =
            StrUtil::trim(defaultSongName) +
            StrUtil::padLeft(
                std::to_string(lockedSequencer->getSelectedSongIndex() + 1),
                "0", 2);

        song->setName(songName);
    }

    if (focusedFieldName == "loop")
    {
        openScreenById(ScreenId::LoopSongScreen);
    }
    else if (focusedFieldName == "song")
    {
        openScreenById(ScreenId::SongWindow);
    }
    else if (focusedFieldName == "tempo" || focusedFieldName == "tempo-source")
    {
        openScreenById(ScreenId::IgnoreTempoChangeScreen);
    }
}

void SongScreen::turnWheel(const int increment)
{
    const auto lockedSequencer = sequencer.lock();

    const auto selectedStepIndex = lockedSequencer->getSelectedSongStepIndex();
    const bool isEndOfSong =
        lockedSequencer->isSelectedSongStepIndexEndOfSong();
    const auto song = lockedSequencer->getSelectedSong();
    const auto stepCount = song->getStepCount();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("sequence") != std::string::npos)
    {
        if (isEndOfSong)
        {
            song->insertStep(SongStepIndex(stepCount));
            lockedSequencer->setSelectedSongStepIndex(SongStepIndex(stepCount));

            if (!song->isUsed())
            {
                song->setUsed(true);
                const auto songName =
                    StrUtil::trim(defaultSongName) +
                    StrUtil::padLeft(
                        std::to_string(lockedSequencer->getSelectedSongIndex() +
                                       1),
                        "0", 2);

                song->setName(songName);
            }
            return;
        }

        const auto selectedStep = song->getStep(selectedStepIndex).lock();
        selectedStep->setSequence(selectedStep->getSequenceIndex() + increment);
        lockedSequencer->setSelectedSongStepIndex(selectedStepIndex);
    }
    else if (focusedFieldName.find("reps") != std::string::npos)
    {
        if (isEndOfSong)
        {
            return;
        }

        const auto selectedStep = song->getStep(selectedStepIndex).lock();
        selectedStep->setRepeats(selectedStep->getRepeats() + increment);
        displaySteps();
    }
    else if (focusedFieldName == "song")
    {
        lockedSequencer->setSelectedSongIndex(
            lockedSequencer->getSelectedSongIndex() + increment);
    }
    else if (focusedFieldName == "tempo" &&
             !sequencer.lock()->getTransport()->isTempoSourceSequence())
    {
        sequencer.lock()->getTransport()->setTempo(
            sequencer.lock()->getTransport()->getTempo() + increment * 0.1);
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer.lock()->getTransport()->setTempoSourceIsSequence(increment >
                                                                   0);
        displayTempoSource();
        displayTempo();
    }
    else if (focusedFieldName == "loop")
    {
        song->setLoopEnabled(increment > 0);
        displayLoop();
    }
    else if (focusedFieldName == "step1")
    {
        const auto currentStepIndex =
            lockedSequencer->getSelectedSongStepIndex();

        lockedSequencer->setSelectedSongStepIndex(currentStepIndex + increment);
    }
}

void SongScreen::function(const int i)
{
    const auto lockedSequencer = sequencer.lock();
    const auto transport = lockedSequencer->getTransport();

    if (transport->isPlaying())
    {
        return;
    }

    const auto song = lockedSequencer->getSelectedSong();
    const auto selectedSongStepIndex =
        lockedSequencer->getSelectedSongStepIndex();
    const bool isEndOfSong =
        lockedSequencer->isSelectedSongStepIndexEndOfSong();

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ConvertSongToSeqScreen);
            break;
        case 4:
            if (transport->isPlaying() || isEndOfSong)
            {
                return;
            }

            song->deleteStep(selectedSongStepIndex);
            displaySteps();
            displayNow0();
            displayNow1();
            displayNow2();
            displayTempo();
            break;
        case 5:
        {
            if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
                focusedFieldName != "step1" && focusedFieldName != "sequence1")
            {
                return;
            }

            song->insertStep(selectedSongStepIndex);

            lockedSequencer->setSelectedSongStepIndex(selectedSongStepIndex);

            if (!song->isUsed())
            {
                song->setUsed(true);
                const auto songName =
                    StrUtil::trim(defaultSongName) +
                    StrUtil::padLeft(
                        std::to_string(lockedSequencer->getSelectedSongIndex() +
                                       1),
                        "0", 2);
                song->setName(songName);
            }

            displaySongName();
            displaySteps();
            displayTempo();
            break;
        }
        default:;
    }
}

void SongScreen::displayTempo() const
{
    findField("tempo")->setText(
        Util::tempoString(sequencer.lock()->getTransport()->getTempo()));
}

void SongScreen::displayLoop() const
{
    const auto song = sequencer.lock()->getSelectedSong();
    findField("loop")->setText(song->isLoopEnabled() ? "ON" : "OFF");
}

void SongScreen::displaySteps() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto song = lockedSequencer->getSelectedSong();
    const auto selectedStepIndex = lockedSequencer->getSelectedSongStepIndex();
    const int songStepCount = song->getStepCount();

    const auto stepArray =
        std::vector{findField("step0"), findField("step1"), findField("step2")};
    const auto sequenceArray = std::vector{
        findField("sequence0"), findField("sequence1"), findField("sequence2")};
    const auto repsArray =
        std::vector{findField("reps0"), findField("reps1"), findField("reps2")};

    for (int i = 0; i < 3; i++)
    {
        const int stepIndex = (i + selectedStepIndex) - 1;

        if (stepIndex < 0 || stepIndex >= songStepCount)
        {
            stepArray[i]->setText("");
            repsArray[i]->setText("");

            if (stepIndex == songStepCount)
            {
                sequenceArray[i]->setText("   (end of song)");
                continue;
            }

            sequenceArray[i]->setText("");
            continue;
        }

        stepArray[i]->setText(std::to_string(stepIndex + 1));

        const auto step = song->getStep(SongStepIndex(stepIndex)).lock();

        auto seqname =
            lockedSequencer->getSequence(step->getSequenceIndex())->getName();

        sequenceArray[i]->setText(
            StrUtil::padLeft(std::to_string(step->getSequenceIndex() + 1), "0",
                             2) +
            "-" + seqname);

        int repeatCount = step->getRepeats();

        if (i == 1 && lockedSequencer->getTransport()->isPlaying())
        {
            repeatCount -=
                lockedSequencer->getTransport()->getPlayedStepRepetitions();
        }

        repsArray[i]->setText(std::to_string(repeatCount));
    }
}

void SongScreen::displayTempoSource() const
{
    findField("tempo-source")
        ->setText(sequencer.lock()->getTransport()->isTempoSourceSequence()
                      ? "SEQ"
                      : "MAS");
}

void SongScreen::displayNow0() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto transport = lockedSequencer->getTransport();
    const auto song = lockedSequencer->getSelectedSong();
    const auto selectedSongStepIndex =
        lockedSequencer->getSelectedSongStepIndex();

    int pastBars = 0;

    for (int i = 0; i < selectedSongStepIndex; i++)
    {
        if (i >= song->getStepCount())
        {
            break;
        }

        const auto step = song->getStep(SongStepIndex(i)).lock();
        const auto seq = lockedSequencer->getSequence(step->getSequenceIndex());

        if (!seq->isUsed())
        {
            continue;
        }

        const auto bars = (seq->getLastBarIndex() + 1) * step->getRepeats();

        pastBars += bars;
    }

    if (const auto songSequenceIndex = lockedSequencer->getSongSequenceIndex();
        songSequenceIndex != NoSequenceIndex)
    {
        pastBars += transport->getPlayedStepRepetitions() *
                    (lockedSequencer->getSequence(songSequenceIndex)
                         ->getLastBarIndex() + 1);
    }

    findField("now0")->setTextPadded(
        transport->getCurrentBarIndex() + 1 + pastBars, "0");
}

void SongScreen::displayNow1() const
{
    findField("now1")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void SongScreen::displayNow2() const
{
    findField("now2")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentClockNumber(), "0");
}

void SongScreen::displaySongName() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto song = lockedSequencer->getSelectedSong();
    findField("song")->setText(
        StrUtil::padLeft(
            std::to_string(lockedSequencer->getSelectedSongIndex() + 1), "0",
            2) +
        "-" + song->getName());
}

void SongScreen::setDefaultSongName(const std::string &s)
{
    defaultSongName = s;
}

std::string SongScreen::getDefaultSongName()
{
    return defaultSongName;
}
