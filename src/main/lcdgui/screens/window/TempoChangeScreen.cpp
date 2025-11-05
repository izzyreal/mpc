#include "TempoChangeScreen.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/TimeSignature.hpp"

#include "sequencer/Track.hpp"

#include "lcdgui/HorizontalBar2.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;

TempoChangeScreen::TempoChangeScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "tempo-change", layerIndex)
{
    for (int i = 0; i < 3; i++)
    {
        int w = 35;
        int h = 5;
        int x = 191;
        int y = 22 + (i * 9);

        auto rect = MRECT(x, y, x + w, y + h);
        bars.push_back(addChildT<HorizontalBar2>(rect));
    }
}

void TempoChangeScreen::open()
{
    findLabel("initial-tempo")->setLocation(140, 10);

    for (auto &bar : bars)
    {
        bar->Hide(true);
    }

    findField("a0")->setAlignment(Alignment::Centered);
    findField("a1")->setAlignment(Alignment::Centered);
    findField("a2")->setAlignment(Alignment::Centered);

    auto events = sequencer->getActiveSequence()->getTempoChangeEvents();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        auto row = stoi(focusedFieldName.substr(1));
        {
            if (row + offset >= events.size())
            {
                offset = 0;
                ls->setFocus("a0");
            }
        }
    }
    else
    {
        offset = 0;
    }

    initVisibleEvents();

    displayInitialTempo();
    displayTempoChange0();
    displayTempoChange1();
    displayTempoChange2();
    displayTempoChangeOn();
}

void TempoChangeScreen::initVisibleEvents()
{
    auto seq = sequencer->getActiveSequence();

    for (auto &t : visibleTempoChanges)
    {
        if (t)
        {
            t->deleteObserver(this);
        }
    }

    visibleTempoChanges = std::vector<std::shared_ptr<TempoChangeEvent>>(3);

    auto allTce = seq->getTempoChangeEvents();

    for (int i = 0; i < 3; i++)
    {
        if (i + offset < allTce.size())
        {
            visibleTempoChanges[i] = allTce[i + offset];
        }

        if (allTce.size() <= i + offset + 1)
        {
            for (int j = i + 1; j < 2; j++)
            {
                visibleTempoChanges[j] = std::shared_ptr<TempoChangeEvent>();
            }

            break;
        }
    }
}

void TempoChangeScreen::displayInitialTempo()
{
    auto seq = sequencer->getActiveSequence();
    findField("initial-tempo")
        ->setText(Util::tempoString(seq->getInitialTempo()));
}

void TempoChangeScreen::displayTempoChangeOn()
{
    auto sequence = sequencer->getActiveSequence();
    findField("tempo-change")
        ->setText(sequence->isTempoChangeOn() ? "YES" : "NO");
}

void TempoChangeScreen::displayTempoChange0()
{
    auto sequence = sequencer->getActiveSequence();
    bars[0]->Hide(false);

    auto tce = visibleTempoChanges[0];
    findField("a0")->setText(std::to_string(offset + 1));
    auto timeSig = sequence->getTimeSignature();

    int value =
        tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
    findField("b0")->setTextPadded(value, "0");
    value = tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
    findField("c0")->setTextPadded(value, "0");
    value = tce->getClock(timeSig.getDenominator());
    findField("d0")->setTextPadded(value, "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e0")->setText(ratioStr);

    double initialTempo = sequence->getInitialTempo();
    int ratio = tce->getRatio();

    double tempo = initialTempo * double(ratio) * 0.001;

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

void TempoChangeScreen::displayTempoChange1()
{
    auto tce = visibleTempoChanges[1];

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

    auto sequence = sequencer->getActiveSequence();
    auto timeSig = sequence->getTimeSignature();

    findField("b1")->setTextPadded(
        tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
    findField("c1")->setTextPadded(
        tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1,
        "0");
    findField("d1")->setTextPadded(tce->getClock(timeSig.getDenominator()),
                                   "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e1")->setText(ratioStr);

    auto tempo = std::clamp(
        sequence->getInitialTempo() * (tce->getRatio() * 0.001), 30.0, 300.0);

    findField("f1")->setText(Util::tempoString(tempo));
    bars[1]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::displayTempoChange2()
{
    auto tce = visibleTempoChanges[2];

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
        if (!visibleTempoChanges[1])
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

    auto sequence = sequencer->getActiveSequence();
    auto timeSig = sequence->getTimeSignature();
    findField("b2")->setTextPadded(
        tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
    findField("c2")->setTextPadded(
        tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1,
        "0");
    findField("d2")->setTextPadded(tce->getClock(timeSig.getDenominator()),
                                   "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
    ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
    ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
    findField("e2")->setText(ratioStr);

    auto tempo = std::clamp(
        sequence->getInitialTempo() * tce->getRatio() * 0.001, 30.0, 300.0);

    findField("f2")->setText(Util::tempoString(tempo));
    bars[2]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::left()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        if (focusedFieldName[0] == 'a')
        {
            ls->setFocus("tempo-change");
            return;
        }
    }

    ScreenComponent::left();
}

void TempoChangeScreen::right()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        if (focusedFieldName[0] == 'f')
        {
            ls->setFocus("initial-tempo");
            return;
        }
    }

    ScreenComponent::right();
}

void TempoChangeScreen::function(int j)
{
    ScreenComponent::function(j);

    init();

    auto yPos = -1;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        yPos = stoi(focusedFieldName.substr(1, 2));
    }

    auto seq = sequencer->getActiveSequence();

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

            initVisibleEvents();

            displayTempoChange0();
            displayTempoChange1();
            displayTempoChange2();

            ls->setFocus("a" + std::to_string(yPos));
            break;
        case 2:
        {
            auto nowDetected = -1;
            for (int i = 0; i < tceList.size(); i++)
            {
                if (tceList[i]->getTick() == sequencer->getTickPosition())
                {
                    nowDetected = i;
                    break;
                }
            }

            if (nowDetected == -1)
            {
                std::shared_ptr<Event> tce =
                    seq->addTempoChangeEvent(sequencer->getTickPosition());
                initVisibleEvents();
                displayTempoChange0();
                displayTempoChange1();
                displayTempoChange2();

                ls->setFocus(std::string("a" + std::to_string(yPos)));
            }
            else
            {
                if (nowDetected > offset + 3 || nowDetected < offset)
                {
                    setOffset(nowDetected);
                }

                ls->setFocus(focusedFieldName.substr(0, 1) +
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
                seq->addTempoChangeEvent(seq->getLastTick() - 1);
            }
            else if (tceList.size() > 1)
            {
                if (focusedFieldName.length() != 2)
                {
                    return;
                }

                auto lCurrent = current.lock();
                auto lPrevious = previous.lock();

                if (yPos + offset == 0)
                {
                    if (lCurrent->getTick() == 1)
                    {
                        return;
                    }

                    seq->addTempoChangeEvent(next.lock()->getTick() - 1);
                }
                else if (yPos + offset > 0)
                {
                    if (lCurrent->getTick() - 1 == lPrevious->getTick())
                    {
                        return;
                    }

                    seq->addTempoChangeEvent(lCurrent->getTick() - 1);
                }
            }

            initVisibleEvents();
            displayTempoChange0();
            displayTempoChange1();
            displayTempoChange2();

            ls->setFocus(focusedFieldName);
            break;
        }
    }
}

void TempoChangeScreen::init()
{
    auto seq = sequencer->getActiveSequence();
    auto tceList = seq->getTempoChangeEvents();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    auto yPos = stoi(focusedFieldName.substr(1, 2));

    int nextPosition = yPos + offset + 1;

    if (tceList.size() > nextPosition)
    {
        next = tceList[nextPosition];
    }
    else
    {
        next.reset();
    }

    int currentPosition = yPos + offset;

    if (currentPosition + 1 > tceList.size())
    {
        return;
    }

    current = tceList[currentPosition];

    auto previousPosition = yPos + offset - 1;

    if (previousPosition >= 0)
    {
        previous = tceList[previousPosition];
    }
}

void TempoChangeScreen::turnWheel(int j)
{

    auto seq = sequencer->getActiveSequence();
    auto tceList = seq->getTempoChangeEvents();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tempo-change")
    {
        seq->setTempoChangeOn(j > 0);
        displayTempoChangeOn();
        return;
    }
    else if (focusedFieldName == "initial-tempo")
    {
        auto tce = tceList[0];
        seq->setInitialTempo(seq->getInitialTempo() + (j * 0.1));
        displayInitialTempo();
        displayTempoChange0();
        displayTempoChange1();
        displayTempoChange2();
        return;
    }

    auto ts = seq->getTimeSignature();

    for (int i = 0; i < 3; i++)
    {
        auto event = visibleTempoChanges[i];

        bool mayChangePosition = true;

        if (i == 0 && offset == 0)
        {
            mayChangePosition = false;
        }

        if (mayChangePosition && focusedFieldName == "b" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneBar(next.lock().get());
            }
            else
            {
                event->minusOneBar(previous.lock().get());
            }
        }
        else if (mayChangePosition &&
                 focusedFieldName == "c" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneBeat(next.lock().get());
            }
            else
            {
                event->minusOneBeat(previous.lock().get());
            }
        }
        else if (mayChangePosition &&
                 focusedFieldName == "d" + std::to_string(i))
        {
            if (j > 0)
            {
                event->plusOneClock(next.lock().get());
            }
            else
            {
                event->minusOneClock(previous.lock().get());
            }
        }
        else if (focusedFieldName == "e" + std::to_string(i))
        {
            event->setRatio(event->getRatio() + j);
        }
        else if (focusedFieldName == "f" + std::to_string(i))
        {
            auto ratio = (event->getTempo() + j * 0.1) / seq->getInitialTempo();
            event->setRatio((int)round(ratio * 1000.0));
        }

        if (focusedFieldName.length() == 2 &&
            stoi(focusedFieldName.substr(1)) == i)
        {
            if (i == 0)
            {
                displayTempoChange0();
            }
            else if (i == 1)
            {
                displayTempoChange1();
            }
            else if (i == 2)
            {
                displayTempoChange2();
            }

            break;
        }
    }
}

void TempoChangeScreen::down()
{
    init();

    auto tce1 = visibleTempoChanges[1];
    auto tce2 = visibleTempoChanges[2];

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tempo-change")
    {
        ls->setFocus("e0");
    }
    else if (focusedFieldName == "initial-tempo")
    {
        ls->setFocus("f0");
    }

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    auto yPos = stoi(focusedFieldName.substr(1, 2));

    if ((yPos == 1 && !tce1) || (yPos == 2 && !tce2))
    {
        return;
    }
    else if (yPos == 1 && !tce2)
    {
        ls->setFocus("a2");
        return;
    }
    else if (yPos == 2)
    {
        setOffset(offset + 1);

        auto sequence = sequencer->getActiveSequence();

        if (offset + yPos == sequence->getTempoChangeEvents().size() &&
            focusedFieldName[0] != 'a')
        {
            ls->setFocus("a2");
        }

        return;
    }

    auto paramToFocus = focusedFieldName.substr(0, 1);

    if ((yPos == 0 && !tce1) || (yPos == 1 && !tce2))
    {
        paramToFocus = "a";
    }

    ls->setFocus(std::string(paramToFocus + std::to_string(yPos + 1)));
}

void TempoChangeScreen::up()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() != 2)
    {
        return;
    }

    auto yPos = stoi(focusedFieldName.substr(1, 2));

    if (yPos == 0)
    {
        if (offset == 0)
        {
            if (focusedFieldName == "e0")
            {
                ls->setFocus("tempo-change");
            }
            else if (focusedFieldName == "f0")
            {
                ls->setFocus("initial-tempo");
            }

            return;
        }
        setOffset(offset - 1);
        return;
    }

    ls->setFocus(focusedFieldName.substr(0, 1) + std::to_string(yPos - 1));
    return;
}

void TempoChangeScreen::setOffset(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    offset = i;

    initVisibleEvents();
    displayTempoChange0();
    displayTempoChange1();
    displayTempoChange2();
}
