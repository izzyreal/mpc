#include "TrMoveScreen.hpp"

#include "EventsScreen.hpp"
#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

TrMoveScreen::TrMoveScreen(Mpc &mpc, const int layerIndex)
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
        const auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
        eventsScreen->setFromSq(sequencer->getActiveSequenceIndex() + i);
        displaySq();
        displayTrFields();
        displayTrLabels();
    }
}

void TrMoveScreen::up()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        goUp();
    }
}

void TrMoveScreen::down()
{

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
    switch (i)
    {
        // Intentional fall-through
        case 0:
        case 1:
        case 3:
        {
            const auto eventsScreen =
                mpc.screens->get<ScreenId::EventsScreen>();
            eventsScreen->tab = i;
            ls->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
            break;
        }
        case 4:
            if (isSelected())
            {
                cancel();
            }
            break;
        case 5:
        {
            const auto focusedFieldName = getFocusedFieldNameOrThrow();

            if (focusedFieldName == "sq")
            {
                break;
            }

            if (isSelected())
            {
                const auto sequence = sequencer->getActiveSequence();
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

void TrMoveScreen::displayTrLabels() const
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

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    auto sequence = sequencer->getActiveSequence();

    if (tr0Index >= 0)
    {
        tr0Name = sequence->getTrack(tr0Index)->getName();
        tr0 += "Tr:" + StrUtil::padLeft(std::to_string(tr0Index + 1), "0", 2) +
               "-" + tr0Name;
    }
    else
    {
        tr0 = "";
    }
    if (tr1Index < 64)
    {
        tr1Name = sequence->getTrack(tr1Index)->getName();
        tr1 += "Tr:" + StrUtil::padLeft(std::to_string(tr1Index + 1), "0", 2) +
               "-" + tr1Name;
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

void TrMoveScreen::displayTrFields() const
{
    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();
    const auto sequence = sequencer->getActiveSequence();

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

        findField("tr")->setText(
            "Tr:" +
            StrUtil::padLeft(std::to_string(selectedTrackIndex + 1), "0", 2) +
            "-" + tr0Name);
    }
    else
    {
        findLabel("selecttrack")->Hide(false);
        findLabel("tomove")->Hide(false);
        findField("tr")->setLocation(108, 26);
        const auto trackName = sequence->getTrack(currentTrackIndex)->getName();
        const auto trackIndex =
            StrUtil::padLeft(std::to_string(currentTrackIndex + 1), "0", 2);
        findField("tr")->setText("Tr:" + trackIndex + "-" + trackName);
    }
}

void TrMoveScreen::displaySq() const
{
    const auto sequence = sequencer->getActiveSequence();
    findField("sq")->setText(StrUtil::padLeft(
        std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2));
    const auto sequenceName = "-" + sequence->getName();
    findLabel("sq-name")->setText(sequenceName);
}

bool TrMoveScreen::isSelected() const
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

void TrMoveScreen::insert(sequencer::Sequence *s)
{
    s->moveTrack(selectedTrackIndex, currentTrackIndex);
    selectedTrackIndex = -1;
    displayTrLabels();
    displayTrFields();
    ls->setFunctionKeysArrangement(1);
    SetDirty();
}
