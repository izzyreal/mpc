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
                            return sequencer->getTransport()->getTempo();
                        },
                        [&](auto)
                        {
                            displayTempo();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTransport()->isPlaying();
                        },
                        [&](auto isPlaying)
                        {
                            findField("sequence1")->setBlinking(isPlaying);
                            findField("reps1")->setBlinking(isPlaying);
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
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "step1" || focusedFieldName == "sequence1" ||
        focusedFieldName == "reps1")
    {
        if (offset == -1 || sequencer->getTransport()->isPlaying())
        {
            return;
        }

        setOffset(offset - 1);
        sequencer->setActiveSequenceIndex(sequencer->getSongSequenceIndex(),
                                          true);
    }
    else
    {
        ScreenComponent::up();
    }
}

void SongScreen::left()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sequence1")
    {
        ls->setFocus("step1");
    }
    else if (focusedFieldName == "reps1")
    {
        ls->setFocus("sequence1");
    }
    else if (focusedFieldName == "step1")
    {
        ls->setFocus("loop");
    }
    else
    {
        ScreenComponent::left();
    }
}

void SongScreen::right()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sequence1")
    {
        ls->setFocus("reps1");
    }
    else if (focusedFieldName == "step1")
    {
        ls->setFocus("sequence1");
    }
    else if (focusedFieldName == "loop")
    {
        ls->setFocus("step1");
    }
    else
    {
        ScreenComponent::right();
    }
}

void SongScreen::openWindow()
{
    if (sequencer->getTransport()->isPlaying())
    {
        return;
    }

    const auto song = sequencer->getSong(activeSongIndex);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("now") == std::string::npos && !song->isUsed())
    {
        song->setUsed(true);
        const auto songName =
            StrUtil::trim(defaultSongName) +
            StrUtil::padLeft(std::to_string(activeSongIndex + 1), "0", 2);
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

void SongScreen::down()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "step1" || focusedFieldName == "sequence1" ||
        focusedFieldName == "reps1")
    {
        const auto song = sequencer->getSong(activeSongIndex);

        if (offset == song->getStepCount() - 1 ||
            sequencer->getTransport()->isPlaying())
        {
            return;
        }

        setOffset(offset + 1);
        sequencer->setActiveSequenceIndex(sequencer->getSongSequenceIndex(),
                                          true);
    }
    else
    {
        ScreenComponent::down();
    }
}

void SongScreen::turnWheel(int i)
{
    const auto song = sequencer->getSong(activeSongIndex);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("sequence") != std::string::npos)
    {
        if (offset + 1 > song->getStepCount() - 1)
        {
            song->insertStep(offset + 1);

            if (!song->isUsed())
            {
                song->setUsed(true);
                const auto songName =
                    StrUtil::trim(defaultSongName) +
                    StrUtil::padLeft(std::to_string(activeSongIndex + 1), "0",
                                     2);
                song->setName(songName);
            }

            displaySongName();
            displaySteps();
            displayTempo();
            return;
        }

        const auto step = song->getStep(offset + 1).lock();

        step->setSequence(step->getSequence() + i);
        sequencer->setActiveSequenceIndex(step->getSequence(), true);
        displayNow0();
        displayNow1();

        displayNow2();
        displaySteps();
    }
    else if (focusedFieldName.find("reps") != std::string::npos)
    {
        if (offset + 1 > song->getStepCount() - 1)
        {
            return;
        }

        song->getStep(offset + 1)
            .lock()
            ->setRepeats(song->getStep(offset + 1).lock()->getRepeats() + i);
        displaySteps();
    }
    else if (focusedFieldName == "song")
    {
        setActiveSongIndex(activeSongIndex + i);
        setOffset(-1);
        displayLoop();

        if (song->isUsed() && song->getStepCount() != 0)
        {
            sequencer->setActiveSequenceIndex(
                song->getStep(0).lock()->getSequence(), true);
        }
    }
    else if (focusedFieldName == "tempo" &&
             !sequencer->getTransport()->isTempoSourceSequenceEnabled())
    {
        sequencer->getTransport()->setTempo(
            sequencer->getTransport()->getTempo() + i * 0.1);
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer->getTransport()->setTempoSourceSequence(i > 0);
        displayTempoSource();
        displayTempo();
    }
    else if (focusedFieldName == "loop")
    {
        const auto song = sequencer->getSong(activeSongIndex);
        song->setLoopEnabled(i > 0);
        displayLoop();
    }
    else if (focusedFieldName == "step1")
    {
        setOffset(offset + i);
    }
}

void SongScreen::function(int i)
{
    if (sequencer->getTransport()->isPlaying())
    {
        return;
    }

    const auto song = sequencer->getSong(activeSongIndex);

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ConvertSongToSeqScreen);
            break;
        case 4:
            if (sequencer->getTransport()->isPlaying())
            {
                return;
            }

            song->deleteStep(offset + 1);
            displaySteps();
            displayNow0();
            displayNow1();
            displayNow2();
            displayTempo();
            break;
        case 5:
        {
            const auto focusedFieldName = getFocusedFieldNameOrThrow();

            if (focusedFieldName != "step1" && focusedFieldName != "sequence1")
            {
                return;
            }

            song->insertStep(offset + 1);

            auto candidate = offset + 1;

            if (candidate + 1 >= song->getStepCount())
            {
                candidate -= 1;
            }

            setOffset(candidate);

            if (!song->isUsed())
            {
                song->setUsed(true);
                const auto songName =
                    StrUtil::trim(defaultSongName) +
                    StrUtil::padLeft(std::to_string(activeSongIndex + 1), "0",
                                     2);
                song->setName(songName);
            }

            displaySongName();
            displaySteps();
            displayTempo();
            break;
        }
    }
}

void SongScreen::displayTempo() const
{
    findField("tempo")->setText(
        Util::tempoString(sequencer->getTransport()->getTempo()));
}

void SongScreen::displayLoop() const
{
    const auto song = sequencer->getSong(activeSongIndex);
    findField("loop")->setText(song->isLoopEnabled() ? "YES" : "NO");
}

void SongScreen::displaySteps() const
{
    const auto song = sequencer->getSong(activeSongIndex);
    const int steps = song->getStepCount();

    const auto stepArray =
        std::vector{findField("step0"), findField("step1"), findField("step2")};
    const auto sequenceArray = std::vector{
        findField("sequence0"), findField("sequence1"), findField("sequence2")};
    const auto repsArray =
        std::vector{findField("reps0"), findField("reps1"), findField("reps2")};

    for (int i = 0; i < 3; i++)
    {
        const int stepIndex = i + offset;

        if (stepIndex >= 0 && stepIndex < steps)
        {
            stepArray[i]->setText(std::to_string(stepIndex + 1));
            auto seqname =
                sequencer
                    ->getSequence(
                        song->getStep(stepIndex).lock()->getSequence())
                    ->getName();
            sequenceArray[i]->setText(
                StrUtil::padLeft(
                    std::to_string(
                        song->getStep(stepIndex).lock()->getSequence() + 1),
                    "0", 2) +
                "-" + seqname);
            repsArray[i]->setText(
                std::to_string(song->getStep(stepIndex).lock()->getRepeats()));
        }
        else
        {
            stepArray[i]->setText("");
            sequenceArray[i]->setText(stepIndex == steps ? "   (end of song)"
                                                         : "");
            repsArray[i]->setText("");
        }
    }
}

void SongScreen::displayTempoSource() const
{
    findField("tempo-source")
        ->setText(sequencer->getTransport()->isTempoSourceSequenceEnabled()
                      ? "SEQ"
                      : "MAS");
}

void SongScreen::displayNow0() const
{
    int pastBars = 0;

    const auto song = sequencer->getSong(activeSongIndex);

    for (int i = 0; i < offset + 1; i++)
    {
        if (i >= song->getStepCount())
        {
            break;
        }

        const auto step = song->getStep(i).lock();
        const auto seq = sequencer->getSequence(step->getSequence());

        if (!seq->isUsed())
        {
            continue;
        }

        const auto bars = (seq->getLastBarIndex() + 1) * step->getRepeats();

        pastBars += bars;
    }

    pastBars += sequencer->getTransport()->getPlayedStepRepetitions() *
                (sequencer->getActiveSequence()->getLastBarIndex() + 1);

    findField("now0")->setTextPadded(
        sequencer->getTransport()->getCurrentBarIndex() + 1 + pastBars, "0");
}

void SongScreen::displayNow1() const
{
    findField("now1")->setTextPadded(
        sequencer->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void SongScreen::displayNow2() const
{
    findField("now2")->setTextPadded(
        sequencer->getTransport()->getCurrentClockNumber(), "0");
}

void SongScreen::displaySongName() const
{
    const auto song = sequencer->getSong(activeSongIndex);
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(activeSongIndex + 1), "0", 2) + "-" +
        song->getName());
}

void SongScreen::setOffset(int i)
{
    if (i < -1)
    {
        i = -1;
    }

    const auto song = sequencer->getSong(activeSongIndex);

    if (i >= song->getStepCount() - 1)
    {
        i = song->getStepCount() - 1;
    }

    offset = i;

    displaySteps();
    displayTempo();
}

void SongScreen::setActiveSongIndex(int i)
{
    activeSongIndex = std::clamp(i, 0, 19);

    displaySongName();
    displaySteps();
    displayTempoSource();
    displayTempo();
    displayNow0();
    displayNow1();
    displayNow2();
}

void SongScreen::setDefaultSongName(const std::string &s)
{
    defaultSongName = s;
}

std::string SongScreen::getDefaultSongName()
{
    return defaultSongName;
}

int SongScreen::getOffset() const
{
    return offset;
}

int SongScreen::getActiveSongIndex() const
{
    return activeSongIndex;
}
