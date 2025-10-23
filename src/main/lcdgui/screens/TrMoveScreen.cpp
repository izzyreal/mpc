#include "TrMoveScreen.hpp"

#include "EventsScreen.hpp"

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

TrMoveScreen::TrMoveScreen(mpc::Mpc& mpc, const int layerIndex)
    : ScreenComponent(mpc, "tr-move", layerIndex)
{
}

void TrMoveScreen::open()
{
    findBackground()->SetDirty();
    findLabel("selecttrack")->setText("Select track");
    findLabel("tomove")->setText("to move.");

    displaySq();
    displayTrFields();
    displayTrLabels();
}

void TrMoveScreen::turnWheel(int i)
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("tr") != std::string::npos && i > 0)
    {
        goUp();
    }
    else if (focusedFieldName.find("tr") != std::string::npos && i < 0)
    {
        goDown();
    }
    else if (focusedFieldName == "sq")
    {
        auto eventsScreen = mpc.screens->get<EventsScreen>();
        eventsScreen->setFromSq(sequencer.lock()->getActiveSequenceIndex() + i);
        displaySq();
        displayTrFields();
        displayTrLabels();
    }
}

void TrMoveScreen::up()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        goUp();
    }
}

void TrMoveScreen::down()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        goDown();
    }
    else
    {
        ScreenComponent::down();
        ls->setFunctionKeysArrangement(1);
    }
}

void TrMoveScreen::left()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
 
    if (focusedFieldName == "sq")
    {
        return;
    }

    if (isSelected())
    {
        return;
    }

    ScreenComponent::left();
    ls->setFunctionKeysArrangement(0);
}

void TrMoveScreen::right()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        return;
    }

    ScreenComponent::right();
    ls->setFunctionKeysArrangement(1);
}

void TrMoveScreen::function(int i)
{
    init();
    switch (i)
    {
        // Intentional fall-through
        case 0:
        case 1:
        case 3:
            {
                auto eventsScreen = mpc.screens->get<EventsScreen>();
                eventsScreen->tab = i;
                mpc.getLayeredScreen()->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
                break;
            }
        case 4:
            if (isSelected())
            {
                cancel();
            }
            break;
        case 5: {
                    const auto focusedFieldName = getFocusedFieldNameOrThrow();

                    if (focusedFieldName == "sq")
                    {
                        break;
                    }

                    if (isSelected())
                    {
                        auto sequence = sequencer.lock()->getActiveSequence();
                        insert(sequence.get());
                    }
                    else
                    {
                        select();
                    }
                    break;
                }
    }
}

void TrMoveScreen::displayTrLabels()
{
    std::string tr0;
    std::string tr1;
    std::string tr0Name;
    std::string tr1Name;

    auto tr0Index = currentTrackIndex - 1;
    auto tr1Index = 0;

    if (isSelected() && tr0Index >= selectedTrackIndex)
    {
        tr0Index++;
    }

    tr1Index = tr0Index + 2;

    if (isSelected())
    {
        tr1Index--;

        if (tr0Index == selectedTrackIndex)
        {
            tr0Index--;
        }

        if (tr1Index == selectedTrackIndex)
        {
            tr1Index++;
        }
    }

    auto eventsScreen = mpc.screens->get<EventsScreen>();
    auto sequence = sequencer.lock()->getActiveSequence();

    if (tr0Index >= 0)
    {
        tr0Name = sequence->getTrack(tr0Index)->getName();
        tr0 += "Tr:" + StrUtil::padLeft(std::to_string(tr0Index + 1), "0", 2) + "-" + tr0Name;
    }
    else
    {
        tr0 = "";
    }
    if (tr1Index < 64)
    {
        tr1Name = sequence->getTrack(tr1Index)->getName();
        tr1 += "Tr:" + StrUtil::padLeft(std::to_string(tr1Index + 1), "0", 2) + "-" + tr1Name;
    }
    else
    {
        tr1 = "";
    }

    if (tr0 == "")
    {
        findLabel("tr0")->Hide(true);
    }
    else
    {
        findLabel("tr0")->Hide(false);
        findLabel("tr0")->setText(tr0);
    }

    if (tr1 == "")
    {
        findLabel("tr1")->Hide(true);
    }
    else
    {
        findLabel("tr1")->Hide(false);
        findLabel("tr1")->setText(tr1);
    }
}

void TrMoveScreen::displayTrFields()
{
    auto eventsScreen = mpc.screens->get<EventsScreen>();
    auto sequence = sequencer.lock()->getActiveSequence();

    if (isSelected())
    {
        findLabel("selecttrack")->Hide(true);
        findLabel("tomove")->Hide(true);
        findField("tr")->setLocation(9, 26);

        auto tr0Name = sequence->getTrack(selectedTrackIndex)->getName();

        if (tr0Name.length() < 10)
        {
            tr0Name = StrUtil::padRight(tr0Name, " ", 9) + u8"\u00CD";
        }

        findField("tr")->setText("Tr:" + StrUtil::padLeft(std::to_string(selectedTrackIndex + 1), "0", 2) + "-" + tr0Name);
    }
    else
    {
        findLabel("selecttrack")->Hide(false);
        findLabel("tomove")->Hide(false);
        findField("tr")->setLocation(108, 26);
        auto trackName = sequence->getTrack(currentTrackIndex)->getName();
        auto trackIndex = StrUtil::padLeft(std::to_string(currentTrackIndex + 1), "0", 2);
        findField("tr")->setText("Tr:" + trackIndex + "-" + trackName);
    }
}

void TrMoveScreen::displaySq()
{
    auto sequence = sequencer.lock()->getActiveSequence();
    findField("sq")->setText(StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
    auto sequenceName = "-" + sequence->getName();
    findLabel("sq-name")->setText(sequenceName);
}

bool TrMoveScreen::isSelected()
{
    if (selectedTrackIndex != -1)
    {
        return true;
    }
    return false;
}

void TrMoveScreen::goUp()
{
    if (currentTrackIndex == 0)
    {
        return;
    }

    currentTrackIndex--;

    displayTrLabels();
    displayTrFields();
}

void TrMoveScreen::goDown()
{
    if (currentTrackIndex == 63)
    {
        return;
    }
    currentTrackIndex++;
    displayTrLabels();
    displayTrFields();
}

void TrMoveScreen::select()
{
    selectedTrackIndex = currentTrackIndex;
    displayTrLabels();
    displayTrFields();
    ls->setFunctionKeysArrangement(2);
    SetDirty();
}

void TrMoveScreen::cancel()
{
    selectedTrackIndex = -1;
    displayTrLabels();
    displayTrFields();
    ls->setFunctionKeysArrangement(1);
    SetDirty();
}

void TrMoveScreen::insert(mpc::sequencer::Sequence* s)
{
    s->moveTrack(selectedTrackIndex, currentTrackIndex);
    selectedTrackIndex = -1;
    displayTrLabels();
    displayTrFields();
    ls->setFunctionKeysArrangement(1);
    SetDirty();
}
