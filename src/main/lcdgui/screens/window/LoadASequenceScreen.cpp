#include "LoadASequenceScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

LoadASequenceScreen::LoadASequenceScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-a-sequence", layerIndex)
{
}

void LoadASequenceScreen::open()
{
	auto loadScreen = mpc.screens->get<LoadScreen>("load");
    auto midFile = loadScreen->getSelectedFile();
    sequence_or_error parsedMidFile = mpc.getDisk().lock()->readMid2(midFile);
    
    if (parsedMidFile.has_value())
    {
        auto usedSeqs = sequencer.lock()->getUsedSequenceIndexes();
        int index;
        
        for (index = 0; index < 98; index++)
        {
            if (find(begin(usedSeqs), end(usedSeqs), index) == end(usedSeqs))
                break;
        }
        
        loadInto = index;
        displayFile();
    }
    displayLoadInto();
}

void LoadASequenceScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("load-into") == 0)
		setLoadInto(loadInto + i);
}

void LoadASequenceScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("load");
		sequencer.lock()->clearPlaceHolder();
		break;
	case 4:
		sequencer.lock()->movePlaceHolderTo(loadInto);
		sequencer.lock()->setActiveSequenceIndex(loadInto);
		openScreen("sequencer");
		break;
	}
}

void LoadASequenceScreen::setLoadInto(int i)
{
	if (i < 0 || i > 98)
		return;

    loadInto = i;
	displayLoadInto();
}

void LoadASequenceScreen::displayLoadInto()
{
	findField("load-into").lock()->setTextPadded(loadInto + 1, "0");
	findLabel("name").lock()->setText("-" + sequencer.lock()->getSequence(loadInto).lock()->getName());
}

void LoadASequenceScreen::displayFile()
{
	auto s = sequencer.lock()->getPlaceHolder().lock();
	findLabel("file").lock()->setText("File:" + StrUtil::toUpper(s->getName()) + ".MID");
}
