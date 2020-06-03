#include <controls/vmpc/DirectToDiskRecorderControls.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/DirectToDiskSettings.hpp>

#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Song.hpp>

#include <audio/server/AudioServer.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::controls::vmpc;
using namespace std;

DirectToDiskRecorderControls::DirectToDiskRecorderControls()
	: AbstractVmpcControls()
{
}

void DirectToDiskRecorderControls::turnWheel(int i)
{
    init();
	checkAllTimes(i);

	if (param.compare("rate") == 0)
	{
		d2dRecorderGui->setSampleRate(d2dRecorderGui->getSampleRate() + i);
	}
	if (param.compare("record") == 0)
	{
		d2dRecorderGui->setRecord(d2dRecorderGui->getRecord() + i);
	}
	else if (param.compare("sq") == 0)
	{
		d2dRecorderGui->setSq(d2dRecorderGui->getSq() + i);
	}
	else if (param.compare("song") == 0)
	{
		d2dRecorderGui->setSong(d2dRecorderGui->getSong() + i);
	}
	else if (param.compare("splitlr") == 0)
	{
		d2dRecorderGui->setSplitLR(i > 0);
	}
	else if (param.compare("offline") == 0)
	{
		d2dRecorderGui->setOffline(i > 0);
	}
    else if(param.compare("outputfolder") == 0)
	{
		auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));
		nameScreen->setName(d2dRecorderGui->getOutputfolder());
		nameScreen->setNameLimit(8);
		nameScreen->parameterName = "outputfolder";
        ls.lock()->openScreen("name");
    }
}

void DirectToDiskRecorderControls::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
	{
		auto seq = d2dRecorderGui->getSq();
		auto outputFolder = d2dRecorderGui->getOutputfolder();
		vector<int> rates{ 44100, 48000, 88200 };
		auto rate = rates[d2dRecorderGui->getSampleRate()];

		if (!d2dRecorderGui->isOffline())
		{
			rate = ams.lock()->getAudioServer()->getSampleRate();
		}

		auto split = false;
		auto sequence = sequencer.lock()->getSequence(seq).lock();

		switch (d2dRecorderGui->getRecord())
		{
		case 0:
		{
			ls.lock()->openScreen("sequencer");
			sequence->setLoopEnabled(false);
			auto lengthInFrames = mpc::sequencer::SeqUtil::sequenceFrameLength(sequence.get(), 0, sequence->getLastTick(), rate);
			auto settings = make_unique<mpc::audiomidi::DirectToDiskSettings>(lengthInFrames, outputFolder, split, rate);
			ams.lock()->prepareBouncing(settings.get());
			sequencer.lock()->playFromStart();
			break;
		}
		case 1:
		{
			ls.lock()->openScreen("sequencer");
			auto lengthInFrames = mpc::sequencer::SeqUtil::loopFrameLength(sequence.get(), rate);
			auto settings = make_unique<mpc::audiomidi::DirectToDiskSettings>(lengthInFrames, outputFolder, split, rate);
			sequence->setLoopEnabled(false);
			sequencer.lock()->move(sequence->getLoopStart());
			ams.lock()->prepareBouncing(settings.get());
			sequencer.lock()->play();
			break;
		}
		case 2:
		{
			ls.lock()->openScreen("sequencer");
			auto lengthInFrames = mpc::sequencer::SeqUtil::sequenceFrameLength(sequence.get(), d2dRecorderGui->getTime0(), d2dRecorderGui->getTime1(), rate);
			auto settings = make_unique<mpc::audiomidi::DirectToDiskSettings>(lengthInFrames, outputFolder, split, rate);
			sequence->setLoopEnabled(false);
			sequencer.lock()->move(d2dRecorderGui->getTime0());
			ams.lock()->prepareBouncing(settings.get());
			sequencer.lock()->play();
			break;
		}
		case 3:
		{
			auto song = sequencer.lock()->getSong(d2dRecorderGui->getSong()).lock();
			
			if (!song->isUsed())
			{
				return;
			}

			auto lengthInFrames = mpc::sequencer::SeqUtil::songFrameLength(song.get(), sequencer.lock().get(), rate);
			auto settings = make_unique<mpc::audiomidi::DirectToDiskSettings>(lengthInFrames, outputFolder, split, rate);
			
			ls.lock()->openScreen("song");
			
			sequencer.lock()->setSongModeEnabled(true);

			auto songScreen = dynamic_pointer_cast<SongScreen>(Screens::getScreenComponent("song"));
			songScreen->setLoop(false);

			ams.lock()->prepareBouncing(settings.get());
			sequencer.lock()->playFromStart();
			break;
		}
		case 4:
			ls.lock()->openScreen("recordjam");
			break;
		}
	}
	}
}
