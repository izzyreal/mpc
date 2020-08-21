#include "CopyTrackScreen.hpp"

#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

CopyTrackScreen::CopyTrackScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "copy-track", layerIndex)
{
}

void CopyTrackScreen::open()
{
	displayTr0();
	displayTr1();
}

void CopyTrackScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 4:
	{
		auto seqIndex = sequencer.lock()->getActiveSequenceIndex();
		sequencer.lock()->copyTrack(tr0, tr1, seqIndex, seqIndex);
		openScreen("sequencer");
		break;
	}
	}
}

void CopyTrackScreen::turnWheel(int i)
{
	init();
	if (param.find("0") != string::npos)
	{
		setTr0(tr0 + i);
	}
	else if (param.find("1") != string::npos)
	{
		setTr1(tr1 + i);
	}
}


void CopyTrackScreen::setTr0(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	tr0 = i;
	displayTr0();
}

void CopyTrackScreen::setTr1(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	tr1 = i;
	displayTr1();
}


void CopyTrackScreen::displayTr0()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tr0Name = seq->getTrack(tr0).lock()->getName();
	findField("tr0").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(tr0 + 1), " ", 2) + "-" + tr0Name);
}

void CopyTrackScreen::displayTr1()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tr1Name = seq->getTrack(tr1).lock()->getName();
	findField("tr1").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(tr1 + 1), " ", 2) + "-" + tr1Name);
}
