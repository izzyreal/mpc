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

LoadASequenceScreen::LoadASequenceScreen(mpc::Mpc &mpc, const int layerIndex)
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

    const sequence_or_error parsedMidFile = mpc.getDisk()->readMid2(midFile);

    if (parsedMidFile.has_value())
    {
        auto usedSeqs = sequencer->getUsedSequenceIndexes();
        int index;

        for (index = 0; index < 98; index++)
        {
            if (find(begin(usedSeqs), end(usedSeqs), index) == end(usedSeqs))
            {
                break;
            }
        }

        loadInto = index;
        displayFile();
    }

    displayLoadInto();
}

void LoadASequenceScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "load-into")
    {
        setLoadInto(loadInto + i);
    }
}

void LoadASequenceScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->closeCurrentScreen();
            sequencer->clearPlaceHolder();
            break;
        case 4:
            sequencer->movePlaceHolderTo(loadInto);
            sequencer->setActiveSequenceIndex(loadInto);
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}

void LoadASequenceScreen::setLoadInto(int i)
{
    loadInto = std::clamp(i, 0, 98);
    displayLoadInto();
}

void LoadASequenceScreen::displayLoadInto()
{
    findField("load-into")->setTextPadded(loadInto + 1, "0");
    findLabel("name")->setText("-" +
                               sequencer->getSequence(loadInto)->getName());
}

void LoadASequenceScreen::displayFile()
{
    const auto s = sequencer->getPlaceHolder();
    findLabel("file")->setText("File:" + StrUtil::toUpper(s->getName()) +
                               ".MID");
}
