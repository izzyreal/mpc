#include "TempoChangeScreen.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <lcdgui/Label.hpp>
#include <lcdgui/HorizontalBar.hpp>

#include <Util.hpp>

using namespace mpc::sequencer;
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
			t.lock()->deleteObserver(this);
		}
	}
}

void TempoChangeScreen::initVisibleEvents()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();

	for (auto& t : visibleTempoChangeEvents)
	{
		if (t.lock()) {
			t.lock()->deleteObserver(this);
		}
	}

	visibleTempoChangeEvents = vector<weak_ptr<TempoChangeEvent>>(3);
	
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
				visibleTempoChangeEvents[j] = weak_ptr<TempoChangeEvent>();
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

void TempoChangeScreen::left()
{
	init();
	if (param.length() == 2) {
		if (param[0] == 'a') {
			ls.lock()->setFocus("tempochange");
			return;
		}
	}
	BaseControls::left();
}

void TempoChangeScreen::right()
{
	init();
	if (param.length() == 2) {
		if (param[0] == 'f') {
			ls.lock()->setFocus("initialtempo");
			return;
		}
	}
	BaseControls::right();
}

void TempoChangeScreen::function(int j)
{
	BaseControls::function(j);
	
	auto yPos = -1;

	if (param.length() == 2) {
		yPos = stoi(param.substr(1, 2));
	}

	int nowDetected;
	std::shared_ptr<TempoChangeEvent> tce;
	auto seq = sequencer.lock()->getActiveSequence().lock();

	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto tceList = seq->getTempoChangeEvents();

	switch (j) {
	case 1:
		if (yPos + swGui->getTempoChangeOffset() >= tceList.size())
		{
			return;
		}
		
		if (tceList[swGui->getTempoChangeOffset() + yPos].lock()->getStepNumber() == 0)
		{
			return;
		}
		
		seq->removeTempoChangeEvent(swGui->getTempoChangeOffset() + yPos);
		seq->sortTempoChangeEvents();

		if (swGui->getTempoChangeOffset() + yPos == tceList.size())
		{
			swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() - 1);
		}

		ls.lock()->openScreen("tempochange");
		ls.lock()->setFocus(string("a" + to_string(yPos)));
		break;
	case 2:
		nowDetected = -1;
		for (int i = 0; i < tceList.size(); i++)
		{
			if (tceList[i].lock()->getTick() == sequencer.lock()->getTickPosition())
			{
				nowDetected = i;
				break;
			}
		}
		
		if (nowDetected == -1)
		{
			auto tce = seq->addTempoChangeEvent().lock();
			tce->setTick(sequencer.lock()->getTickPosition());
			seq->sortTempoChangeEvents();
		}
		else
		{
			if (nowDetected > swGui->getTempoChangeOffset() + 3 || nowDetected < swGui->getTempoChangeOffset())
			{
				swGui->setTempoChangeOffset(nowDetected);
			}
			
			ls.lock()->setFocus(param.substr(0, 1) + to_string(nowDetected - swGui->getTempoChangeOffset()));
		}
		break;
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		tceList = seq->getTempoChangeEvents();
		
		if (tceList.size() == 1)
		{
			tce = seq->addTempoChangeEvent().lock();
			tce->setTick(seq->getLastTick());
			tce->setStepNumber(1);
		}
		else if (tceList.size() > 1)
		{
			if (param.length() != 2)
			{
				return;
			}
			
			auto lCurrent = current.lock();
			auto lNext = next.lock();
			auto lPrevious = previous.lock();
			
			if (yPos + swGui->getTempoChangeOffset() == 0)
			{
				if (lCurrent->getTick() == 1)
				{
					return;
				}
				tce = seq->addTempoChangeEvent().lock();
				tce->setTick(lNext->getTick() - 1);
			}
			else if (yPos + swGui->getTempoChangeOffset() > 0) {
				if (lCurrent->getTick() - 1 == lPrevious->getTick())
					return;
				tce = seq->addTempoChangeEvent().lock();
				tce->setTick(lCurrent->getTick() - 1);
			}
		}
		seq->sortTempoChangeEvents();
		ls.lock()->openScreen("tempochange");
		break;
	}
}

void TempoChangeScreen::init()
{
	BaseControls::init();
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tceList = seq->getTempoChangeEvents();

	if (param.length() != 2)
	{
		return;
	}

	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	auto yPos = stoi(param.substr(1, 2));

	int nextPosition = yPos + swGui->getTempoChangeOffset() + 1;

	if (tceList.size() > nextPosition)
	{
		next = tceList[nextPosition];
	}

	int currentPosition = yPos + swGui->getTempoChangeOffset();

	if (currentPosition + 1 > tceList.size())
	{
		return;
	}

	current = tceList[currentPosition];

	auto previousPosition = yPos + swGui->getTempoChangeOffset() - 1;

	if (previousPosition >= 0)
	{
		previous = tceList[previousPosition];
	}
}

void TempoChangeScreen::turnWheel(int j)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tceList = seq->getTempoChangeEvents();

	if (param.compare("tempochange") == 0)
	{
		seq->setTempoChangeOn(j > 0);
	}
	else if (param.compare("initialtempo") == 0)
	{
		auto tce = tceList[0].lock();
		seq->setInitialTempo(BCMath(seq->getInitialTempo().toDouble() + (j / 10.0)));
	}
	
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	
	for (int i = 0; i < 3; i++)
	{
		auto ts = seq->getTimeSignature();
		auto tce = swGui->getVisibleTempoChanges()[i].lock();

		if (param.compare("b" + to_string(i)) == 0)
		{
			if (j > 0)
			{
				tce->plusOneBar(ts.getNumerator(), ts.getDenominator(), next.lock().get());
			}
			else
			{
				tce->minusOneBar(ts.getNumerator(), ts.getDenominator(), previous.lock().get());
			}
		}
		else if (param.compare("c" + to_string(i)) == 0)
		{
			if (j > 0)
			{
				tce->plusOneBeat(ts.getDenominator(), next.lock().get());
			}
			else
			{
				tce->minusOneBeat(ts.getDenominator(), previous.lock().get());
			}
		}
		else if (param.compare("d" + to_string(i)) == 0)
		{
			if (j > 0)
			{
				tce->plusOneClock(next.lock().get());
			}
			else {
				tce->minusOneClock(previous.lock().get());
			}
		}
		else if (param.compare("e" + to_string(i)) == 0)
		{
			tce->setRatio(tce->getRatio() + j);
		}
	}
}

void TempoChangeScreen::down()
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto tce1 = swGui->getVisibleTempoChanges()[1].lock();
	auto tce2 = swGui->getVisibleTempoChanges()[2].lock();

	if (param.compare("tempochange") == 0)
	{
		ls.lock()->setFocus("e0");
	}
	else if (param.compare("initialtempo") == 0)
	{
		ls.lock()->setFocus("f0");
	}
	
	if (param.length() != 2)
	{
		return;
	}
	
	auto yPos = stoi(param.substr(1, 2));

	if ((yPos == 1 && !tce1) || (yPos == 2 && !tce2))
	{
		return;
	}
	else if (yPos == 1 && !tce2)
	{
		ls.lock()->setFocus("a2");
		return;
	}
	else if (yPos == 2)
	{
		swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() + 1);

		auto sequence = sequencer.lock()->getActiveSequence().lock();

		if (swGui->getTempoChangeOffset() + yPos == sequence->getTempoChangeEvents().size() && param[0] != 'a')
		{
			ls.lock()->setFocus("a2");
		}
		return;
	}

	auto paramToFocus = param.substr(0, 1);

	if ((yPos == 0 && !tce1) || (yPos == 1 && !tce2))
	{
		paramToFocus = "a";
	}

	ls.lock()->setFocus(string(paramToFocus + to_string(yPos + 1)));
}

void TempoChangeScreen::up()
{
	init();

	if (param.length() != 2) {
		return;
	}

	auto yPos = stoi(param.substr(1, 2));

	if (yPos == 0)
	{
		auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
		if (swGui->getTempoChangeOffset() == 0)
		{
			if (param.compare("e0") == 0)
			{
				ls.lock()->setFocus("tempochange");
			}
			else if (param.compare("f0") == 0)
			{
				ls.lock()->setFocus("initialtempo");
			}
			return;
		}
		swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() - 1);
		return;
	}

	ls.lock()->setFocus(param.substr(0, 1) + to_string(yPos - 1));
	return;
}
