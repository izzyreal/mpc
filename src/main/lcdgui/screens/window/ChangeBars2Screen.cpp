#include "ChangeBars2Screen.hpp"
#include "sequencer/Transport.hpp"

#include "sequencer/Sequence.hpp"

#include "StrUtil.hpp"

#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ChangeBars2Screen::ChangeBars2Screen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "change-bars-2", layerIndex)
{
}

void ChangeBars2Screen::open()
{
    setNewBars(std::max(
        sequencer.lock()->getSelectedSequence()->getLastBarIndex(), 0));
    displayCurrent();
    displayNewBars();
}

void ChangeBars2Screen::displayCurrent() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    findLabel("current")->setText(std::to_string(seq->getBarCount()));
}

void ChangeBars2Screen::function(const int i)
{
    ScreenComponent::function(i);
    const auto seq = sequencer.lock()->getSelectedSequence();

    switch (i)
    {
        case 2:
            openScreenById(ScreenId::SequencerScreen); // Required for desired
                                                       // screen transitions
            openScreenById(ScreenId::ChangeBarsScreen);
            break;
        case 4:
        {
            auto lastBarIndex = seq->getLastBarIndex();
            auto changed = false;

            if (newBars < lastBarIndex)
            {
                seq->deleteBars(newBars + 1, lastBarIndex);
                lastBarIndex = seq->getLastBarIndex();
                changed = true;
            }

            if (newBars > lastBarIndex)
            {
                seq->insertBars(newBars - lastBarIndex, BarIndex(lastBarIndex + 1));
                changed = true;
            }

            if (changed)
            {
                sequencer.lock()->getTransport()->setPosition(0);
            }

            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void ChangeBars2Screen::displayNewBars() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    const auto message0 = findLabel("message0");
    const auto message1 = findLabel("message1");

    findField("newbars")->setText(
        StrUtil::padLeft(std::to_string(newBars + 1), " ", 3));

    if (newBars == std::max(seq->getLastBarIndex(), 0))
    {
        message0->setText("");
        message1->setText("");
    }
    else if (newBars > std::max(seq->getLastBarIndex(), 0))
    {
        message0->setText("Pressing DO IT will add");
        message1->setText("blank bars after last bar.");
    }
    else if (newBars < std::max(seq->getLastBarIndex(), 0))
    {
        message0->setText("Pressing DO IT will truncate");
        message1->setText("bars after last bar.");
    }
}

void ChangeBars2Screen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newbars")
    {
        setNewBars(newBars + i);
    }
}

void ChangeBars2Screen::setNewBars(const int i)
{
    newBars = std::clamp(i, 0, 998);
    displayNewBars();
}
