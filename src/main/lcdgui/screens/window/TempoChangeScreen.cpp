#include "TempoChangeScreen.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <lcdgui/Label.hpp>
#include <lcdgui/HorizontalBar.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;

TempoChangeScreen::TempoChangeScreen(const int& layer)
	: ScreenComponent("tempochange", layer)
{
}

void TempoChangeScreen::open()
{

	bars = ls.lock()->getHorizontalBarsTempoChangeEditor();
	
	for (auto& bar :bars) {
		bar.lock()->Hide(true);
	}

	a0Field = findField("a0");
	a1Field = findField("a1");
	a2Field = findField("a2");
	b0Field = findField("b0");
	b1Field = findField("b1");
	b2Field = findField("b2");
	c0Field = findField("c0");
	c1Field = findField("c1");
	c2Field = findField("c2");
	d0Field = findField("d0");
	d1Field = findField("d1");
	d2Field = findField("d2");
	e0Field = findField("e0");
	e1Field = findField("e1");
	e2Field = findField("e2");
	f0Field = findField("f0");
	f1Field = findField("f1");
	f2Field = findField("f2");
	a0Label = findLabel("a0");
	a1Label = findLabel("a1");
	a2Label = findLabel("a2");
	b0Label = findLabel("b0");
	b1Label = findLabel("b1");
	b2Label = findLabel("b2");
	c0Label = findLabel("c0");
	c1Label = findLabel("c1");
	c2Label = findLabel("c2");
	d0Label = findLabel("d0");
	d1Label = findLabel("d1");
	d2Label = findLabel("d2");
	e0Label = findLabel("e0");
	e1Label = findLabel("e1");
	e2Label = findLabel("e2");
	f0Label = findLabel("f0");
	f1Label = findLabel("f1");
	f2Label = findLabel("f2");

	initVisibleEvents();

	initVisibleEvents();
	displayInitialTempo();
	displayTempoChange0();
	displayTempoChange1();
	displayTempoChange2();
	displayTempoChangeOn();
}

void TempoChangeScreen::close()
{
	for (auto& h : bars)
	{
		h.lock()->Hide(true);
	}

	for (auto& t : visibleTempoChangeEvents)
	{
		if (t.lock()) {
			t.lock()->deleteObservers();
		}
	}
}

void TempoChangeScreen::function(int i)
{
	BaseControls::function(i);
}

void TempoChangeScreen::turnWheel(int i)
{
	init();

}

void TempoChangeScreen::initVisibleEvents()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();

	visibleTempoChangeEvents = vector<weak_ptr<mpc::sequencer::TempoChangeEvent>>(3);
	
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto allTce = seq->getTempoChangeEvents();
	
	for (int i = 0; i < 3; i++)
	{
		if (i + swGui->getTempoChangeOffset() < allTce.size())
		{
			visibleTempoChangeEvents[i] = allTce[i + swGui->getTempoChangeOffset()];
			auto tce = visibleTempoChangeEvents[i].lock();
		
			if (tce)
			{
				tce->addObserver(this);
			}
		}
		
		if (allTce.size() <= i + swGui->getTempoChangeOffset() + 1)
		{
			for (int j = i + 1; j < 2; j++)
			{
				visibleTempoChangeEvents[j] = weak_ptr<mpc::sequencer::TempoChangeEvent>();
			}
			break;
		}
	}

	swGui->setVisibleTempoChanges(visibleTempoChangeEvents);
}

void TempoChangeScreen::displayInitialTempo()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	string tempoStr = seq->getInitialTempo().toString();
	tempoStr = mpc::Util::replaceDotWithSmallSpaceDot(tempoStr);
	findField("initialtempo").lock()->setText(tempoStr);
}

void TempoChangeScreen::displayTempoChangeOn()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	findField("tempochange").lock()->setText(sequence->isTempoChangeOn() ? "YES" : "NO");
}

void TempoChangeScreen::displayTempoChange0()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	bars[1].lock()->Hide(false);
	
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto tce = swGui->getVisibleTempoChanges()[0].lock();
	a0Field.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	auto timeSig = sequence->getTimeSignature();
	int value = tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	
	b0Field.lock()->setTextPadded(value, "0");
	value = tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	c0Field.lock()->setTextPadded(value, "0");
	value = tce->getClock(timeSig.getNumerator(), timeSig.getDenominator());
	d0Field.lock()->setTextPadded(value, "0");

	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e0Field.lock()->setText(ratioStr);

	auto tempo = sequence->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	
	if (tempo < 30)
	{
		tempo = 30.0;
	}
	else if (tempo > 300)
	{
		tempo = 300.0;
	}

	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f0Field.lock()->setText(tempoStr);
	bars[1].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void TempoChangeScreen::displayTempoChange1()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	int size = swGui->getVisibleTempoChanges().size();
	auto tce = swGui->getVisibleTempoChanges()[1].lock();
	
	if (!tce)
	{
		a1Field.lock()->setText("END");
		b1Field.lock()->Hide(true);
		c1Field.lock()->Hide(true);
		d1Field.lock()->Hide(true);
		e1Field.lock()->Hide(true);
		f1Field.lock()->Hide(true);
		b1Label.lock()->Hide(true);
		c1Label.lock()->Hide(true);
		d1Label.lock()->Hide(true);
		e1Label.lock()->Hide(true);
		f1Label.lock()->Hide(true);
		bars[2].lock()->Hide(true);
		return;
	}
	
	b1Field.lock()->Hide(false);
	c1Field.lock()->Hide(false);
	d1Field.lock()->Hide(false);
	e1Field.lock()->Hide(false);
	f1Field.lock()->Hide(false);
	b1Label.lock()->Hide(false);
	c1Label.lock()->Hide(false);
	d1Label.lock()->Hide(false);
	e1Label.lock()->Hide(false);
	f1Label.lock()->Hide(false);
	bars[2].lock()->Hide(false);

	a1Field.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	auto timeSig = sequence->getTimeSignature();

	b1Field.lock()->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c1Field.lock()->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d1Field.lock()->setTextPadded(tce->getClock(timeSig.getNumerator(), timeSig.getDenominator()), "0");

	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e1Field.lock()->setText(ratioStr);

	auto tempo = sequence->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	
	if (tempo < 30)
	{
		tempo = 30.0;
	}
	else if (tempo > 300) {
		tempo = 300.0;
	}
	
	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f1Field.lock()->setText(tempoStr);

	bars[2].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void TempoChangeScreen::displayTempoChange2()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto tce = swGui->getVisibleTempoChanges()[2].lock();
	
	if (!tce)
	{
		if (!swGui->getVisibleTempoChanges()[1].lock())
		{
			a2Field.lock()->Hide(true);
		}
		else
		{
			a2Field.lock()->Hide(false);
			a2Field.lock()->setText("END");
		}

		b2Field.lock()->Hide(true);
		c2Field.lock()->Hide(true);
		d2Field.lock()->Hide(true);
		e2Field.lock()->Hide(true);
		f2Field.lock()->Hide(true);
		b2Label.lock()->Hide(true);
		c2Label.lock()->Hide(true);
		d2Label.lock()->Hide(true);
		e2Label.lock()->Hide(true);
		f2Label.lock()->Hide(true);
		bars[3].lock()->Hide(true);
		return;
	}

	b2Field.lock()->Hide(false);
	c2Field.lock()->Hide(false);
	d2Field.lock()->Hide(false);
	e2Field.lock()->Hide(false);
	f2Field.lock()->Hide(false);
	b2Label.lock()->Hide(false);
	c2Label.lock()->Hide(false);
	d2Label.lock()->Hide(false);
	e2Label.lock()->Hide(false);
	f2Label.lock()->Hide(false);
	bars[3].lock()->Hide(false);
	a2Field.lock()->setText(" " + to_string(tce->getStepNumber() + 1));

	auto sequence = sequencer.lock()->getActiveSequence().lock();
	auto timeSig = sequence->getTimeSignature();
	b2Field.lock()->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c2Field.lock()->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d2Field.lock()->setTextPadded(tce->getClock(timeSig.getNumerator(), timeSig.getDenominator()), "0");

	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e2Field.lock()->setText(ratioStr);

	auto tempo = sequence->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	
	if (tempo < 30)
	{
		tempo = 30.0;
	}
	else if (tempo > 300)
	{
		tempo = 300.0;
	}

	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f2Field.lock()->setText(tempoStr);

	bars[3].lock()->setValue((tempo - 15) * (290 / 975.0));
}


void TempoChangeScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{

	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("tempochange") == 0)
	{
		initVisibleEvents();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
	}
	else if (msg.compare("offset") == 0 || msg.compare("tempochangeadded") == 0 || msg.compare("tick") == 0)
	{
		initVisibleEvents();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
	}
	else if (msg.compare("tempochangeon") == 0)
	{
		displayTempoChangeOn();
	}
	else if (msg.compare("tempo") == 0)
	{
		displayInitialTempo();
	}
	else if (msg.compare("initialtempo") == 0)
	{
		initVisibleEvents();
		displayInitialTempo();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
	}
}
