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
    addReactiveBinding(
        {[&]
         {
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

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getTransport()->isPlaying();
         },
         [&](auto isPlaying)
         {
             findField("sequence1")->setBlinking(isPlaying);
             findField("reps1")->setBlinking(isPlaying);
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
        if (offset == -1 || sequencer.lock()->getTransport()->isPlaying())
        {
            return;
        }

        setOffset(offset - 1);
        sequencer.lock()->setSelectedSequenceIndex(
            sequencer.lock()->getSongSequenceIndex(), true);
    }
    else
    {
        ScreenComponent::up();
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
    if (sequencer.lock()->getTransport()->isPlaying())
    {
        return;
    }

    const auto song = sequencer.lock()->getSong(selectedSongIndex);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("now") == std::string::npos && !song->isUsed())
    {
        song->setUsed(true);
        const auto songName =
            StrUtil::trim(defaultSongName) +
            StrUtil::padLeft(std::to_string(selectedSongIndex + 1), "0", 2);
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
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "step1" || focusedFieldName == "sequence1" ||
        focusedFieldName == "reps1")
    {
        if (const auto song = sequencer.lock()->getSong(selectedSongIndex);
            offset == song->getStepCount() - 1 ||
            sequencer.lock()->getTransport()->isPlaying())
        {
            return;
        }

        setOffset(offset + 1);
        sequencer.lock()->setSelectedSequenceIndex(
            sequencer.lock()->getSongSequenceIndex(), true);
    }
    else
    {
        ScreenComponent::down();
    }
}

void SongScreen::turnWheel(const int increment)
{
    const auto song = sequencer.lock()->getSong(selectedSongIndex);

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("sequence") != std::string::npos)
    {
        if (offset + 1 > song->getStepCount() - 1)
        {
            song->insertStep(offset + 1);

            if (!song->isUsed())
            {
                song->setUsed(true);
                const auto songName =
                    StrUtil::trim(defaultSongName) +
                    StrUtil::padLeft(std::to_string(selectedSongIndex + 1), "0",
                                     2);
                song->setName(songName);
            }

            displaySongName();
            displaySteps();
            displayTempo();
            return;
        }

        const auto step = song->getStep(offset + 1).lock();

        step->setSequence(step->getSequence() + increment);
        sequencer.lock()->setSelectedSequenceIndex(step->getSequence(), true);
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
            ->setRepeats(song->getStep(offset + 1).lock()->getRepeats() +
                         increment);
        displaySteps();
    }
    else if (focusedFieldName == "song")
    {
        setSelectedSongIndex(selectedSongIndex + increment);
        setOffset(-1);
        displayLoop();

        if (song->isUsed() && song->getStepCount() != 0)
        {
            sequencer.lock()->setSelectedSequenceIndex(
                song->getStep(0).lock()->getSequence(), true);
        }
    }
    else if (focusedFieldName == "tempo" &&
             !sequencer.lock()->getTransport()->isTempoSourceSequenceEnabled())
    {
        sequencer.lock()->getTransport()->setTempo(
            sequencer.lock()->getTransport()->getTempo() + increment * 0.1);
    }
    else if (focusedFieldName == "tempo-source")
    {
        sequencer.lock()->getTransport()->setTempoSourceSequence(increment > 0);
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
        setOffset(offset + increment);
    }
}

void SongScreen::function(const int i)
{
    if (sequencer.lock()->getTransport()->isPlaying())
    {
        return;
    }

    const auto song = sequencer.lock()->getSong(selectedSongIndex);

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ConvertSongToSeqScreen);
            break;
        case 4:
            if (sequencer.lock()->getTransport()->isPlaying())
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
            if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
                focusedFieldName != "step1" && focusedFieldName != "sequence1")
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
                    StrUtil::padLeft(std::to_string(selectedSongIndex + 1), "0",
                                     2);
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
    const auto song = sequencer.lock()->getSong(selectedSongIndex);
    findField("loop")->setText(song->isLoopEnabled() ? "YES" : "NO");
}

void SongScreen::displaySteps() const
{
    const auto song = sequencer.lock()->getSong(selectedSongIndex);
    const int steps = song->getStepCount();

    const auto stepArray =
        std::vector{findField("step0"), findField("step1"), findField("step2")};
    const auto sequenceArray = std::vector{
        findField("sequence0"), findField("sequence1"), findField("sequence2")};
    const auto repsArray =
        std::vector{findField("reps0"), findField("reps1"), findField("reps2")};

    for (int i = 0; i < 3; i++)
    {
        if (const int stepIndex = i + offset;
            stepIndex >= 0 && stepIndex < steps)
        {
            stepArray[i]->setText(std::to_string(stepIndex + 1));
            auto seqname =
                sequencer.lock()
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
        ->setText(
            sequencer.lock()->getTransport()->isTempoSourceSequenceEnabled()
                ? "SEQ"
                : "MAS");
}

void SongScreen::displayNow0() const
{
    int pastBars = 0;

    const auto song = sequencer.lock()->getSong(selectedSongIndex);

    for (int i = 0; i < offset + 1; i++)
    {
        if (i >= song->getStepCount())
        {
            break;
        }

        const auto step = song->getStep(i).lock();
        const auto seq = sequencer.lock()->getSequence(step->getSequence());

        if (!seq->isUsed())
        {
            continue;
        }

        const auto bars = (seq->getLastBarIndex() + 1) * step->getRepeats();

        pastBars += bars;
    }

    pastBars +=
        sequencer.lock()->getTransport()->getPlayedStepRepetitions() *
        (sequencer.lock()->getSelectedSequence()->getLastBarIndex() + 1);

    findField("now0")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBarIndex() + 1 + pastBars,
        "0");
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
    const auto song = sequencer.lock()->getSong(selectedSongIndex);
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(selectedSongIndex + 1), "0", 2) + "-" +
        song->getName());
}

void SongScreen::setOffset(int i)
{
    if (i < -1)
    {
        i = -1;
    }

    if (const auto song = sequencer.lock()->getSong(selectedSongIndex);
        i >= song->getStepCount() - 1)
    {
        i = song->getStepCount() - 1;
    }

    offset = i;

    displaySteps();
    displayTempo();
}

void SongScreen::setSelectedSongIndex(const int i)
{
    selectedSongIndex = std::clamp(i, 0, 19);

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

int SongScreen::getSelectedSongIndex() const
{
    return selectedSongIndex;
}
