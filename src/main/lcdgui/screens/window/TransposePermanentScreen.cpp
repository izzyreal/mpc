#include "TransposePermanentScreen.hpp"

#include <lcdgui/screens/TransScreen.hpp>

#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

TransposePermanentScreen::TransposePermanentScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "transpose-permanent", layerIndex)
{
}

void TransposePermanentScreen::function(int i)
{
	ScreenComponent::function(i); // For closing this window
	
	switch (i)
	{
	case 4:
		auto transScreen = dynamic_pointer_cast<TransScreen>(mpc.screens->getScreenComponent("trans"));
		auto all = transScreen->tr == -1;

		vector<int> tracks;
		
		if (all)
		{
			for (int i = 0; i < 64; i++)
				tracks.push_back(i);
		}
		else
		{
			tracks.push_back(transScreen->tr);
		}

		auto seq = sequencer.lock()->getActiveSequence().lock();
		auto firstTick = seq->getFirstTickOfBar(transScreen->bar0);
		auto lastTick = seq->getLastTickOfBar(transScreen->bar1);
		
		for (auto& i : tracks)
		{
			auto t = seq->getTrack(i);
		
			for (auto& n : t.lock()->getNoteEvents())
			{
				auto noteEvent = n.lock();
				
				if (noteEvent->getTick() < firstTick || noteEvent->getTick() > lastTick)
					continue;
			
				noteEvent->setNote(noteEvent->getNote() + transScreen->transposeAmount);
			}
		}

		openScreen("sequencer");
		break;
	}
}
