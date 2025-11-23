#include "LoadASequenceScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens::window;

LoadASequenceScreen::LoadASequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-sequence", layerIndex)
{
}

void LoadASequenceScreen::open()
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto midFile = loadScreen->getSelectedFile();

    if (!StrUtil::eqIgnoreCase(midFile->getExtension(), ".mid"))
    {
        return;
    }

    if (const sequence_or_error parsedMidFile =
            mpc.getDisk()->readMid2(midFile);
        parsedMidFile.has_value())
    {
        auto usedSeqs = sequencer.lock()->getUsedSequenceIndexes();
        int index;

        for (index = MinSequenceIndex; index <= MaxSequenceIndex; index += 1)
        {
            if (find(begin(usedSeqs), end(usedSeqs), index) == end(usedSeqs))
            {
                break;
            }
        }

        loadInto = SequenceIndex(index);
        displayFile();
    }

    displayLoadInto();
}

void LoadASequenceScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "load-into")
    {
        setLoadInto(loadInto + increment);
    }
}

void LoadASequenceScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->closeCurrentScreen();
            sequencer.lock()->clearPlaceHolder();
            break;
        case 4:
            sequencer.lock()->movePlaceHolderTo(loadInto);
            sequencer.lock()->setSelectedSequenceIndex(loadInto, true);
            openScreenById(ScreenId::SequencerScreen);
            break;
        default:;
    }
}

void LoadASequenceScreen::setLoadInto(const SequenceIndex i)
{
    loadInto = std::clamp(i, MinSequenceIndex, MaxSequenceIndex);
    displayLoadInto();
}

void LoadASequenceScreen::displayLoadInto() const
{
    findField("load-into")->setTextPadded(loadInto + 1, "0");
    findLabel("name")->setText("-" +
                               sequencer.lock()->getSequence(loadInto)->getName());
}

void LoadASequenceScreen::displayFile() const
{
    const auto s = sequencer.lock()->getPlaceHolder();
    findLabel("file")->setText("File:" + StrUtil::toUpper(s->getName()) +
                               ".MID");
}
