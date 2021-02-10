#include "LoadASequenceScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <file/mid/MidiReader.hpp>

#include <lcdgui/screens/LoadScreen.hpp>

#include <file/FileUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::file;
using namespace moduru::lang;
using namespace std;

LoadASequenceScreen::LoadASequenceScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-a-sequence", layerIndex)
{
}

void LoadASequenceScreen::open()
{
	auto newSeq = sequencer.lock()->createSeqInPlaceHolder().lock();
	newSeq->init(2);

	auto loadScreen = mpc.screens->get<LoadScreen>("load");
	auto ext = FileUtil::splitName(loadScreen->getSelectedFileName())[1];

	if (ext.compare("mid") == 0 || ext.compare("MID") == 0)
	{

		auto midiReader = mpc::file::mid::MidiReader(loadScreen->getSelectedFile().get(), newSeq);

		midiReader.parseSequence(mpc);

		auto usedSeqs = sequencer.lock()->getUsedSequenceIndexes();
		int index;

		for (index = 0; index < 99; index++)
		{
			bool contains = false;

			for (int i : usedSeqs)
			{
				if (i == index)
				{
					contains = true;
					break;
				}
			}

			if (!contains)
			{
				break;
			}
		}

		loadInto = index;
	}

	displayLoadInto();
	displayFile();
}

void LoadASequenceScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("load-into") == 0)
	{
		setLoadInto(loadInto + i);
	}
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
	{
		return;
	}
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
	findLabel("file").lock()->setText("File:" + StrUtil::toUpper(mpc::disk::AbstractDisk::padFileName16(s->getName())) + ".MID");
	return;
}
