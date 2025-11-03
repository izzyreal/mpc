#include "SongScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Song.hpp"

#include <Util.hpp>

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens;

SongScreen::SongScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "song", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer->getTickPosition();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTempo();
                        },
                        [&](auto)
                        {
                            displayTempo();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->isPlaying();
                        },
                        [&](auto isPlaying)
                        {
                            findField("sequence1")->setBlinking(isPlaying);
                            findField("reps1")->setBlinking(isPlaying);
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTickPosition();
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
        if (offset == -1 || sequencer->isPlaying())
        {
            return;
        }

        setOffset(offset - 1);
        sequencer->setActiveSequenceIndex(sequencer->getSongSequenceIndex());
        sequencer->setBar(0);
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
    if (sequencer->isPlaying())
    {
        return;
    }

    auto song = sequencer->getSong(activeSongIndex);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("now") == std::string::npos && !song->isUsed())
    {
        song->setUsed(true);
        auto songName =
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
        auto song = sequencer->getSong(activeSongIndex);

        if (offset == song->getStepCount() - 1 || sequencer->isPlaying())
        {
            return;
        }

        setOffset(offset + 1);
        sequencer->setActiveSequenceIndex(sequencer->getSongSequenceIndex());
        sequencer->setBar(0);
    }
    else
    {
        ScreenComponent::down();
    }
}

void SongScreen::turnWheel(int i)
{
    auto song = sequencer->getSong(activeSongIndex);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("sequence") != std::string::npos)
    {
        if (offset + 1 > song->getStepCount() - 1)
        {
            song->insertStep(offset + 1);

            if (!song->isUsed())
            {
                song->setUsed(true);
                auto songName =
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

        auto step = song->getStep(offset + 1).lock();

        step->setSequence(step->getSequence() + i);
        sequencer->setActiveSequenceIndex(step->getSequence());
        sequencer->setBar(0);
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
                song->getStep(0).lock()->getSequence());
        }
    }
    else if (focusedFieldName == "tempo" &&
             !sequencer->isTempoSourceSequenceEnabled())
    {
        sequencer->setTempo(sequencer->getTempo() + (i * 0.1));
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer->setTempoSourceSequence(i > 0);
        displayTempoSource();
        displayTempo();
    }
    else if (focusedFieldName == "loop")
    {
        auto song = sequencer->getSong(activeSongIndex);
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
    if (sequencer->isPlaying())
    {
        return;
    }

    auto song = sequencer->getSong(activeSongIndex);

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ConvertSongToSeqScreen);
            break;
        case 4:
            if (sequencer->isPlaying())
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
                auto songName =
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

void SongScreen::displayTempo()
{
    findField("tempo")->setText(Util::tempoString(sequencer->getTempo()));
}

void SongScreen::displayLoop()
{
    auto song = sequencer->getSong(activeSongIndex);
    findField("loop")->setText(song->isLoopEnabled() ? "YES" : "NO");
}

void SongScreen::displaySteps()
{
    auto song = sequencer->getSong(activeSongIndex);
    int steps = song->getStepCount();

    auto stepArray = std::vector<std::shared_ptr<mpc::lcdgui::Field>>{
        findField("step0"), findField("step1"), findField("step2")};
    auto sequenceArray = std::vector<std::shared_ptr<mpc::lcdgui::Field>>{
        findField("sequence0"), findField("sequence1"), findField("sequence2")};
    auto repsArray = std::vector<std::shared_ptr<mpc::lcdgui::Field>>{
        findField("reps0"), findField("reps1"), findField("reps2")};

    for (int i = 0; i < 3; i++)
    {
        int stepIndex = i + offset;

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

void SongScreen::displayTempoSource()
{
    findField("tempo-source")
        ->setText(sequencer->isTempoSourceSequenceEnabled() ? "SEQ" : "MAS");
}

void SongScreen::displayNow0()
{
    int pastBars = 0;

    auto song = sequencer->getSong(activeSongIndex);

    for (int i = 0; i < offset + 1; i++)
    {
        if (i >= song->getStepCount())
        {
            break;
        }

        auto step = song->getStep(i).lock();
        auto seq = sequencer->getSequence(step->getSequence());

        if (!seq->isUsed())
        {
            continue;
        }

        auto bars = (seq->getLastBarIndex() + 1) * step->getRepeats();

        pastBars += bars;
    }

    pastBars += sequencer->getPlayedStepRepetitions() *
                (sequencer->getActiveSequence()->getLastBarIndex() + 1);

    findField("now0")->setTextPadded(
        sequencer->getCurrentBarIndex() + 1 + pastBars, "0");
}

void SongScreen::displayNow1()
{
    findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void SongScreen::displayNow2()
{
    findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");
}

void SongScreen::displaySongName()
{
    auto song = sequencer->getSong(activeSongIndex);
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

    auto song = sequencer->getSong(activeSongIndex);

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

void SongScreen::setDefaultSongName(std::string s)
{
    defaultSongName = s;
}

std::string SongScreen::getDefaultSongName()
{
    return defaultSongName;
}

int SongScreen::getOffset()
{
    return offset;
}

int SongScreen::getActiveSongIndex()
{
    return activeSongIndex;
}
