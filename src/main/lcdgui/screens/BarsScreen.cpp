#include "BarsScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SeqUtil.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"

#include "Util.hpp"

using namespace mpc::lcdgui::screens;

BarsScreen::BarsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "bars", layerIndex)
{
}

void BarsScreen::open()
{
    const auto fromSequence = sequencer.lock()->getSelectedSequence();
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    const auto toSequence = sequencer.lock()->getSequence(eventsScreen->toSq);
    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    const auto userLastBar = userScreen->lastBar;

    const auto lastBarIndexFrom =
        fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;
    const auto maxAfterBarIndex =
        toSequence->isUsed() ? toSequence->getBarCount() : 0;

    if (firstBar > lastBarIndexFrom)
    {
        setFirstBar(lastBarIndexFrom, lastBarIndexFrom);
    }

    if (lastBar > lastBarIndexFrom)
    {
        setLastBar(lastBarIndexFrom, lastBarIndexFrom);
    }

    if (afterBar > maxAfterBarIndex)
    {
        setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
    }

    displayFromSq();
    displayToSq();
    displayFirstBar();
    displayLastBar();
    displayAfterBar();
    displayCopies();
}

void BarsScreen::close()
{
    Util::initSequence(mpc);
}

void BarsScreen::function(const int j)
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    switch (j)
    {
        // Intentional fall-through
        case 0:
        case 2:
        case 3:
            eventsScreen->tab = j;
            ls.lock()->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
            break;
        case 4:
            break;
        case 5:
        {
            copyBars(eventsScreen->toSq, firstBar, lastBar,
                     eventsScreen->copies, afterBar);
            sequencer.lock()->setSelectedSequenceIndex(eventsScreen->toSq,
                                                       true);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void BarsScreen::copyBars(const int toSeqIndex, const int copyFirstBar,
                          const int copyLastBar, const int copyCount,
                          const int copyAfterBar) const
{
    const auto fromSequenceIndex = sequencer.lock()->getSelectedSequenceIndex();
    sequencer::SeqUtil::copyBars(mpc, fromSequenceIndex, toSeqIndex,
                                 copyFirstBar, copyLastBar, copyCount,
                                 copyAfterBar);
}

void BarsScreen::turnWheel(const int increment)
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    const auto userLastBar = userScreen->lastBar;

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "fromsq")
    {
        sequencer.lock()->setSelectedSequenceIndex(
            sequencer.lock()->getSelectedSequenceIndex() + increment, true);

        displayFromSq();

        const auto fromSequence = sequencer.lock()->getSelectedSequence();
        const auto lastBarIndex = fromSequence->isUsed()
                                      ? fromSequence->getLastBarIndex()
                                      : userLastBar;

        if (firstBar > lastBarIndex)
        {
            setFirstBar(lastBarIndex, lastBarIndex);
        }

        if (lastBar > lastBarIndex)
        {
            setLastBar(lastBarIndex, lastBarIndex);
        }
    }
    else if (focusedFieldName == "tosq")
    {
        eventsScreen->setToSq(eventsScreen->toSq + increment);

        displayToSq();

        const auto toSequence =
            sequencer.lock()->getSequence(eventsScreen->toSq);
        const auto maxAfterBarIndex =
            toSequence->isUsed() ? toSequence->getBarCount() : 0;

        if (afterBar > maxAfterBarIndex)
        {
            setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
        }
    }
    else if (focusedFieldName == "afterbar")
    {
        const auto toSequence =
            sequencer.lock()->getSequence(eventsScreen->toSq);

        if (!toSequence->isUsed())
        {
            return;
        }

        setAfterBar(afterBar + increment, toSequence->getBarCount());
    }
    else if (focusedFieldName == "firstbar")
    {
        const auto fromSequence = sequencer.lock()->getSelectedSequence();
        const auto lastBarIndex = fromSequence->isUsed()
                                      ? fromSequence->getLastBarIndex()
                                      : userLastBar;

        setFirstBar(firstBar + increment, lastBarIndex);
    }
    else if (focusedFieldName == "lastbar")
    {
        const auto fromSequence = sequencer.lock()->getSelectedSequence();
        const auto lastBarIndex = fromSequence->isUsed()
                                      ? fromSequence->getLastBarIndex()
                                      : userLastBar;

        setLastBar(lastBar + increment, lastBarIndex);
    }
    else if (focusedFieldName == "copies")
    {
        if (const auto toSequence =
                sequencer.lock()->getSequence(eventsScreen->toSq);
            !toSequence->isUsed())
        {
            Util::initSequence(eventsScreen->toSq, mpc);
        }

        setCopies(eventsScreen->copies + increment);
    }
}

void BarsScreen::displayCopies() const
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    findField("copies")->setTextPadded(eventsScreen->copies, " ");
}

void BarsScreen::displayToSq() const
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    findField("tosq")->setText(std::to_string(eventsScreen->toSq + 1));
}

void BarsScreen::displayFromSq() const
{
    findField("fromsq")->setText(
        std::to_string(sequencer.lock()->getSelectedSequenceIndex().get() + 1));
}

void BarsScreen::displayAfterBar() const
{
    findField("afterbar")->setText(std::to_string(afterBar));
}

void BarsScreen::displayLastBar() const
{
    findField("lastbar")->setText(std::to_string(lastBar + 1));
}

void BarsScreen::displayFirstBar() const
{
    findField("firstbar")->setText(std::to_string(firstBar + 1));
}

void BarsScreen::setLastBar(int i, const int max)
{
    if (i < 0)
    {
        return;
    }

    if (i > max)
    {
        if (lastBar == max)
        {
            return;
        }
        i = max;
    }

    lastBar = i;

    if (lastBar < firstBar)
    {
        setFirstBar(lastBar, max);
    }

    displayLastBar();
}

void BarsScreen::setFirstBar(int i, const int max)
{
    if (i < 0)
    {
        return;
    }
    if (i > max)
    {
        if (firstBar == max)
        {
            return;
        }
        i = max;
    }

    firstBar = i;

    if (firstBar > lastBar)
    {
        setLastBar(firstBar, max);
    }

    displayFirstBar();
}

void BarsScreen::setAfterBar(const int i, const int max)
{
    afterBar = std::clamp(i, 0, max);
    displayAfterBar();
}

void BarsScreen::setCopies(const int i) const
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    eventsScreen->copies = std::clamp(i, 1, 999);
    displayCopies();
}
