#include "TempoChangeScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/TimeSignature.hpp"
#include "sequencer/EventEquals.hpp"

#include "lcdgui/HorizontalBar2.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;

TempoChangeScreen::TempoChangeScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "tempo-change", layerIndex)
{
    for (int i = 0; i < 3; i++)
    {
        constexpr int w = 35;
        constexpr int h = 5;
        constexpr int x = 191;
        const int y = 22 + i * 9;

        auto rect = MRECT(x, y, x + w, y + h);
        bars.push_back(addChildT<HorizontalBar2>(rect));
    }

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getSelectedSequence()
                                ->getTempoChangeTrack()
                                ->getEvents();
                        },
                        [this](auto)
                        {
                            displayTempoChange0();
                            displayTempoChange1();
                            displayTempoChange2();
                        },
                        [](const auto &a, const auto &b)
                        {
                            return eventsEqual(a, b);
                        }});
    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getSelectedSequence()->getInitialTempo();
         },
         [this](auto)
         {
             displayInitialTempo();
             displayTempoChange0();
             displayTempoChange1();
             displayTempoChange2();
         }});
}

void TempoChangeScreen::open()
{
    findLabel("initial-tempo")->setLocation(140, 10);

    for (const auto &bar : bars)
    {
        bar->Hide(true);
    }

    findField("a0")->setAlignment(Alignment::Centered);
    findField("a1")->setAlignment(Alignment::Centered);
    findField("a2")->setAlignment(Alignment::Centered);

    const auto events =
        sequencer.lock()->getSelectedSequence()->getTempoChangeEvents();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
    {
        const auto row = stoi(focusedFieldName.substr(1));
        {
            if (row + offset >= events.size())
            {
                offset = 0;
                ls.lock()->setFocus("a0");
            }
        }
    }
    else
    {
        offset = 0;
    }

    displayInitialTempo();
    displayTempoChangeOn();

    displayTempoChange0();
    displayTempoChange1();
    displayTempoChange2();
}

std::vector<std::shared_ptr<TempoChangeEvent>>
TempoChangeScreen::getVisibleTempoChanges() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    std::vector<std::shared_ptr<TempoChangeEvent>> result(3);

    const auto allTce = seq->getTempoChangeEvents();

    for (int i = 0; i < 3; i++)
    {
        if (i + offset < allTce.size())
        {
            result[i] = allTce[i + offset];
        }

        if (allTce.size() <= i + offset + 1)
        {
            for (int j = i + 1; j < 2; j++)
            {
                result[j] = std::shared_ptr<TempoChangeEvent>();
            }

            break;
        }
    }

    return result;
}

void TempoChangeScreen::displayInitialTempo() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    findField("initial-tempo")
        ->setText(Util::tempoString(seq->getInitialTempo()));
}

void TempoChangeScreen::displayTempoChangeOn() const
{
    const auto sequence = sequencer.lock()->getSelectedSequence();
    findField("tempo-change")
        ->setText(sequence->isTempoChangeOn() ? "YES" : "NO");
}

void TempoChangeScreen::displayTempoChange0() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto sequence = lockedSequencer->getSelectedSequence();
    bars[0]->Hide(false);

    const auto tce = getVisibleTempoChanges()[0];
    findField("a0")->setText(std::to_string(offset + 1));
    const auto timeSig = sequence->getTimeSignature(
        lockedSequencer->getTransport()->getTickPosition());

    int value = tce->getBar(timeSig.numerator, timeSig.denominator) + 1;
    findField("b0")->setTextPadded(value, "0");
    value = tce->getBeat(timeSig.numerator, timeSig.denominator) + 1;
    findField("c0")->setTextPadded(value, "0");
    value = tce->getClock(timeSig.denominator);
    findField("d0")->setTextPadded(value, "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e0")->setText(ratioStr);

    const double initialTempo = sequence->getInitialTempo();
    const int ratio = tce->getRatio();

    double tempo = initialTempo * static_cast<double>(ratio) * 0.001;

    if (tempo < 30.0)
    {
        tempo = 30.0;
    }
    else if (tempo > 300.0)
    {
        tempo = 300.0;
    }

    findField("f0")->setText(Util::tempoString(tempo));
    bars[0]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::displayTempoChange1() const
{
    const auto tce = getVisibleTempoChanges()[1];

    findField("b1")->Hide(!tce);
    findField("c1")->Hide(!tce);
    findField("d1")->Hide(!tce);
    findField("e1")->Hide(!tce);
    findField("f1")->Hide(!tce);
    findLabel("b1")->Hide(!tce);
    findLabel("c1")->Hide(!tce);
    findLabel("d1")->Hide(!tce);
    findLabel("e1")->Hide(!tce);
    findLabel("f1")->Hide(!tce);
    bars[1]->Hide(!tce);

    if (!tce)
    {
        findField("a1")->setText("END");
        return;
    }

    findField("a1")->setText(std::to_string(offset + 2));

    const auto lockedSequencer = sequencer.lock();
    const auto sequence = lockedSequencer->getSelectedSequence();
    const auto timeSig = sequence->getTimeSignature(
        lockedSequencer->getTransport()->getTickPosition());

    findField("b1")->setTextPadded(
        tce->getBar(timeSig.numerator, timeSig.denominator) + 1, "0");
    findField("c1")->setTextPadded(
        tce->getBeat(timeSig.numerator, timeSig.denominator) + 1, "0");
    findField("d1")->setTextPadded(tce->getClock(timeSig.denominator), "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e1")->setText(ratioStr);

    const auto tempo = std::clamp(
        sequence->getInitialTempo() * (tce->getRatio() * 0.001), 30.0, 300.0);

    findField("f1")->setText(Util::tempoString(tempo));
    bars[1]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::displayTempoChange2() const
{
    const auto tce = getVisibleTempoChanges()[2];

    findField("b2")->Hide(!tce);
    findField("c2")->Hide(!tce);
    findField("d2")->Hide(!tce);
    findField("e2")->Hide(!tce);
    findField("f2")->Hide(!tce);
    findLabel("b2")->Hide(!tce);
    findLabel("c2")->Hide(!tce);
    findLabel("d2")->Hide(!tce);
    findLabel("e2")->Hide(!tce);
    findLabel("f2")->Hide(!tce);
    bars[2]->Hide(!tce);

    if (!tce)
    {
        if (!getVisibleTempoChanges()[1])
        {
            findField("a2")->Hide(true);
        }
        else
        {
            findField("a2")->Hide(false);
            findField("a2")->setText("END");
        }
        return;
    }

    findField("a2")->setText(std::to_string(offset + 3));

    const auto lockedSequencer = sequencer.lock();
    const auto sequence = lockedSequencer->getSelectedSequence();
    const auto timeSig = sequence->getTimeSignature(
        lockedSequencer->getTransport()->getTickPosition());
    findField("b2")->setTextPadded(
        tce->getBar(timeSig.numerator, timeSig.denominator) + 1, "0");
    findField("c2")->setTextPadded(
        tce->getBeat(timeSig.numerator, timeSig.denominator) + 1, "0");
    findField("d2")->setTextPadded(tce->getClock(timeSig.denominator), "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e2")->setText(ratioStr);

    const auto tempo = std::clamp(
        sequence->getInitialTempo() * tce->getRatio() * 0.001, 30.0, 300.0);

    findField("f2")->setText(Util::tempoString(tempo));
    bars[2]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::left()
{

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
    {
        if (focusedFieldName[0] == 'a')
        {
            ls.lock()->setFocus("tempo-change");
            return;
        }
    }

    ScreenComponent::left();
}

void TempoChangeScreen::right()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
    {
        if (focusedFieldName[0] == 'f')
        {
            ls.lock()->setFocus("initial-tempo");
            return;
        }
    }

    ScreenComponent::right();
}

void TempoChangeScreen::function(const int j)
{
    ScreenComponent::function(j);

    init();

    auto yPos = -1;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        yPos = stoi(focusedFieldName.substr(1, 2));
    }

    const auto seq = sequencer.lock()->getSelectedSequence();

    auto tceList = seq->getTempoChangeEvents();

    switch (j)
    {
        case 1:
            if (yPos + offset >= tceList.size())
            {
                return;
            }

            // The initial tempo change event can't be removed.
            if (offset + yPos == 0)
            {
                return;
            }

            seq->removeTempoChangeEvent(offset + yPos);

            if (offset + yPos == static_cast<int>(tceList.size()) - 1)
            {
                setOffset(offset - 1);
            }

            displayTempoChange0();
            displayTempoChange1();
            displayTempoChange2();

            ls.lock()->setFocus("a" + std::to_string(yPos));
            break;
        case 2:
        {
            auto nowDetected = -1;
            for (int i = 0; i < tceList.size(); i++)
            {
                if (tceList[i]->getTick() ==
                    sequencer.lock()->getTransport()->getTickPosition())
                {
                    nowDetected = i;
                    break;
                }
            }

            if (nowDetected == -1)
            {
                seq->addTempoChangeEvent(
                    sequencer.lock()->getTransport()->getTickPosition(), 1000);
                displayTempoChange0();
                displayTempoChange1();
                displayTempoChange2();

                ls.lock()->setFocus(std::string("a" + std::to_string(yPos)));
            }
            else
            {
                if (nowDetected > offset + 3 || nowDetected < offset)
                {
                    setOffset(nowDetected);
                }

                ls.lock()->setFocus(focusedFieldName.substr(0, 1) +
                                    std::to_string(nowDetected - offset));
            }
        }
        break;
        case 3:
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 4:
        {
            tceList = seq->getTempoChangeEvents();

            if (tceList.size() == 1)
            {
                seq->addTempoChangeEvent(seq->getLastTick() - 1, 1000);
            }
            else if (tceList.size() > 1)
            {
                if (focusedFieldName.length() != 2)
                {
                    return;
                }

                if (yPos + offset == 0)
                {
                    if (current->getTick() == 1)
                    {
                        return;
                    }

                    seq->addTempoChangeEvent(next->getTick() - 1, 1000);
                }
                else if (yPos + offset > 0)
                {
                    if (current->getTick() - 1 == previous->getTick())
                    {
                        return;
                    }

                    seq->addTempoChangeEvent(current->getTick() - 1, 1000);
                }
            }

            displayTempoChange0();
            displayTempoChange1();
            displayTempoChange2();

            ls.lock()->setFocus(focusedFieldName);
            break;
        }
        default:;
    }
}

void TempoChangeScreen::init()
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    const auto tceList = seq->getTempoChangeEvents();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    const auto yPos = stoi(focusedFieldName.substr(1, 2));

    if (const int nextPosition = yPos + offset + 1;
        tceList.size() > nextPosition)
    {
        next = tceList[nextPosition];
    }
    else
    {
        next.reset();
    }

    const int currentPosition = yPos + offset;

    if (currentPosition + 1 > tceList.size())
    {
        return;
    }

    current = tceList[currentPosition];

    if (const auto previousPosition = yPos + offset - 1; previousPosition >= 0)
    {
        previous = tceList[previousPosition];
    }
}

void TempoChangeScreen::turnWheel(const int j)
{
    init();
    const auto seq = sequencer.lock()->getSelectedSequence();
    const auto tceList = seq->getTempoChangeEvents();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tempo-change")
    {
        seq->setTempoChangeOn(j > 0);
        displayTempoChangeOn();
        return;
    }
    if (focusedFieldName == "initial-tempo")
    {
        auto tce = tceList[0];
        seq->setInitialTempo(seq->getInitialTempo() + j * 0.1);
        return;
    }

    const auto visibleTempoChanges = getVisibleTempoChanges();

    for (int i = 0; i < 3; i++)
    {
        const auto event = visibleTempoChanges[i];

        bool mayChangePosition = true;

        if (i == 0 && offset == 0)
        {
            mayChangePosition = false;
        }

        if (mayChangePosition && focusedFieldName == "b" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneBar(next.get());
            }
            else
            {
                event->minusOneBar(previous.get());
            }
        }
        else if (mayChangePosition &&
                 focusedFieldName == "c" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneBeat(next.get());
            }
            else
            {
                event->minusOneBeat(previous.get());
            }
        }
        else if (mayChangePosition &&
                 focusedFieldName == "d" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneClock(next.get());
            }
            else
            {
                event->minusOneClock(previous.get());
            }
        }
        else if (focusedFieldName == "e" + std::to_string(i))
        {
            event->setRatio(event->getRatio() + j);
        }
        else if (focusedFieldName == "f" + std::to_string(i))
        {
            const auto ratio =
                (event->getTempo() + j * 0.1) / seq->getInitialTempo();
            event->setRatio(static_cast<int>(round(ratio * 1000.0)));
        }
    }
}

void TempoChangeScreen::down()
{
    init();

    const auto visibleTempoChanges = getVisibleTempoChanges();
    const auto tce1 = visibleTempoChanges[1];
    const auto tce2 = visibleTempoChanges[2];

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tempo-change")
    {
        ls.lock()->setFocus("e0");
    }
    else if (focusedFieldName == "initial-tempo")
    {
        ls.lock()->setFocus("f0");
    }

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    const auto yPos = stoi(focusedFieldName.substr(1, 2));

    if ((yPos == 1 && !tce1) || (yPos == 2 && !tce2))
    {
        return;
    }
    if (yPos == 1 && !tce2)
    {
        ls.lock()->setFocus("a2");
        return;
    }
    if (yPos == 2)
    {
        setOffset(offset + 1);

        const auto sequence = sequencer.lock()->getSelectedSequence();

        if (offset + yPos == sequence->getTempoChangeEvents().size() &&
            focusedFieldName[0] != 'a')
        {
            ls.lock()->setFocus("a2");
        }

        return;
    }

    auto paramToFocus = focusedFieldName.substr(0, 1);

    if ((yPos == 0 && !tce1) || (yPos == 1 && !tce2))
    {
        paramToFocus = "a";
    }

    ls.lock()->setFocus(std::string(paramToFocus + std::to_string(yPos + 1)));
}

void TempoChangeScreen::up()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    const auto yPos = stoi(focusedFieldName.substr(1, 2));

    if (yPos == 0)
    {
        if (offset == 0)
        {
            if (focusedFieldName == "e0")
            {
                ls.lock()->setFocus("tempo-change");
            }
            else if (focusedFieldName == "f0")
            {
                ls.lock()->setFocus("initial-tempo");
            }

            return;
        }
        setOffset(offset - 1);
        return;
    }

    ls.lock()->setFocus(focusedFieldName.substr(0, 1) +
                        std::to_string(yPos - 1));
}

void TempoChangeScreen::setOffset(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    offset = i;

    displayTempoChange0();
    displayTempoChange1();
    displayTempoChange2();
}
