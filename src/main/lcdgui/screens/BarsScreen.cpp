#include "BarsScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
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
    const auto fromSequence = sequencer->getActiveSequence();
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    const auto toSequence = sequencer->getSequence(eventsScreen->toSq);
    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    const auto userLastBar = userScreen->lastBar;

    const auto lastBarIndexFrom =
        fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;
    const auto maxAfterBarIndex =
        toSequence->isUsed() ? toSequence->getLastBarIndex() + 1 : 0;

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

void BarsScreen::function(int j)
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    switch (j)
    {
        // Intentional fall-through
        case 0:
        case 2:
        case 3:
            eventsScreen->tab = j;
            ls->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
            break;
        case 4:
            break;
        case 5:
        {
            copyBars(eventsScreen->toSq, firstBar, lastBar,
                     eventsScreen->copies, afterBar);
            sequencer->setActiveSequenceIndex(eventsScreen->toSq, true);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
    }
}

void BarsScreen::copyBars(int toSeqIndex, int copyFirstBar, int copyLastBar,
                          int copyCount, int copyAfterBar) const
{
    const auto fromSequenceIndex = sequencer->getActiveSequenceIndex();
    sequencer::SeqUtil::copyBars(mpc, fromSequenceIndex, toSeqIndex,
                                 copyFirstBar, copyLastBar, copyCount,
                                 copyAfterBar);
}

void BarsScreen::turnWheel(int i)
{

    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    const auto userLastBar = userScreen->lastBar;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "fromsq")
    {
        sequencer->setActiveSequenceIndex(
            sequencer->getActiveSequenceIndex() + i, true);

        displayFromSq();

        const auto fromSequence = sequencer->getActiveSequence();
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
        eventsScreen->setToSq(eventsScreen->toSq + i);

        displayToSq();

        const auto toSequence = sequencer->getSequence(eventsScreen->toSq);
        const auto maxAfterBarIndex =
            toSequence->isUsed() ? toSequence->getLastBarIndex() + 1 : 0;

        if (afterBar > maxAfterBarIndex)
        {
            setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
        }
    }
    else if (focusedFieldName == "afterbar")
    {
        const auto toSequence = sequencer->getSequence(eventsScreen->toSq);

        if (!toSequence->isUsed())
        {
            return;
        }

        setAfterBar(afterBar + i, toSequence->getLastBarIndex() + 1);
    }
    else if (focusedFieldName == "firstbar")
    {
        const auto fromSequence = sequencer->getActiveSequence();
        const auto lastBarIndex = fromSequence->isUsed()
                                      ? fromSequence->getLastBarIndex()
                                      : userLastBar;

        setFirstBar(firstBar + i, lastBarIndex);
    }
    else if (focusedFieldName == "lastbar")
    {
        const auto fromSequence = sequencer->getActiveSequence();
        const auto lastBarIndex = fromSequence->isUsed()
                                      ? fromSequence->getLastBarIndex()
                                      : userLastBar;

        setLastBar(lastBar + i, lastBarIndex);
    }
    else if (focusedFieldName == "copies")
    {
        const auto toSequence = sequencer->getSequence(eventsScreen->toSq);

        if (!toSequence->isUsed())
        {
            mpc::Util::initSequence(eventsScreen->toSq, mpc);
        }

        setCopies(eventsScreen->copies + i);
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
        std::to_string(sequencer->getActiveSequenceIndex() + 1));
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

void BarsScreen::setLastBar(int i, int max)
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

void BarsScreen::setFirstBar(int i, int max)
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

void BarsScreen::setAfterBar(int i, int max)
{
    afterBar = std::clamp(i, 0, max);
    displayAfterBar();
}

void BarsScreen::setCopies(int i) const
{
    const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    eventsScreen->copies = std::clamp(i, 1, 999);
    displayCopies();
}
