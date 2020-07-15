#include "SaveScreen.hpp"

#include <sequencer/Track.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

SaveScreen::SaveScreen(const int layerIndex) 
	: ScreenComponent("save", layerIndex)
{
}

void SaveScreen::open()
{
	displaySize();
	displayType();
	displayFile();
	displayFree();

	findField("device").lock()->setText("SCSI-1");
}

void SaveScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		ls.lock()->openScreen("load");
		break;
	case 2:
		ls.lock()->openScreen("format");
		break;
	case 3:
		//ls.lock()->openScreen("setup");
		break;
	case 5:
	{
		shared_ptr<mpc::sequencer::Sequence> seq;

		auto lProgram = program.lock();
		auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));

		switch (type)
		{
		case 0:
		{
			nameScreen->setName("ALL_SEQ_SONG1");
			ls.lock()->openScreen("save-all-file");
			break;
		}
		case 1:
			seq = sequencer.lock()->getActiveSequence().lock();
			if (!seq->isUsed()) return;

			nameScreen->setName(seq->getName());
			ls.lock()->openScreen("save-a-sequence");
			break;
		case 2:
			nameScreen->setName("ALL_PGMS");
			ls.lock()->openScreen("save-aps-file");
			break;
		case 3:
			nameScreen->setName(mpc::Util::getFileName(lProgram->getName()));
			ls.lock()->openScreen("saveaprogram");
			break;
		case 4:
			if (sampler.lock()->getSoundCount() == 0)
			{
				break;
			}
			nameScreen->setName(sampler.lock()->getSoundName(sampler.lock()->getSoundIndex()));
			ls.lock()->openScreen("save-a-sound");
			break;
		}
	}
	}
}

void SaveScreen::turnWheel(int i)
{
	init();

	if (param.compare("type") == 0)
	{
		setType(type + i);
	}
	else if (param.compare("file") == 0)
	{		
		switch (type)
		{
		case 1:
			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
			break;
		case 3:
		{
			auto nr = sequencer.lock()->getActiveSequence().lock()->getTrack(sequencer.lock()->getActiveTrackIndex()).lock()->getBusNumber();
			sampler.lock()->setDrumBusProgramNumber(nr, sampler.lock()->getDrumBusProgramNumber(nr) + i);
			break;
		}
		case 4:
			sampler.lock()->setSoundIndex(sampler.lock()->getSoundIndex() + i);
			break;
		}
		displayFile();
		displaySize();
	}
}

void SaveScreen::setType(int i)
{
	if (i < 0 || i > 5)
	{
		return;
	}
	
	type = i;

	displayType();
	displayFile();
	displaySize();
}

void SaveScreen::displayType()
{
	findField("type").lock()->setText(types[type]);
}

void SaveScreen::displayFile()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	string file = "";

	switch (type)
	{
	case 0:
		file = "ALL_SEQ_SONG1";
		break;
	case 1:
	{
		auto num = StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2);
		name = seq->getName();
		file = num + "-" + name;
		break;
	}
	case 2:
		file = "ALL_PROGRAM";
		break;
	case 3:
		file = program.lock()->getName();
		break;
	case 4:
		file = string(sampler.lock()->getSoundCount() == 0 ? "" : sampler.lock()->getSound().lock()->getName());
		break;
	}

	findField("file").lock()->setText(file);
}

void SaveScreen::displaySize()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto size = 0;

	switch (type)
	{
	case 0:
		size = sequencer.lock()->getUsedSequenceCount() * 25;
		break;
	case 1:
		size = seq->isUsed() ? 10 + static_cast<int>(seq->getEventCount() * 0.001) : -1;
		break;
	case 2:
		size = sampler.lock()->getProgramCount() * 4;
		break;
	case 3:
		size = 4;
		break;
	case 4:
		size = sampler.lock()->getSoundCount() == 0 ? -1 : (sampler.lock()->getSound().lock()->getSampleData()->size() * 2 * 0.001);
		break;
	case 5:
		size = 512;
		break;
	}

	findLabel("size").lock()->setText(StrUtil::padLeft(to_string(size == -1 ? 0 : size), " ", 6) + "K");
}

void SaveScreen::displayFree()
{
	//auto freeFormatted = moduru::file::FileUtil::getFreeDiskSpaceFormatted(mpc::StartUp::storesPath);
	findLabel("free").lock()->setText("0GB");
}
