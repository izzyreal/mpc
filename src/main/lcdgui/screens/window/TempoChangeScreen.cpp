#include "TempoChangeScreen.hpp"

#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/HorizontalBar.hpp>

#include <Util.hpp>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

TempoChangeScreen::TempoChangeScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "tempo-change", layerIndex)
{
	for (int i = 0; i < 4; i++)
	{
		int w = 33;
		int h = 5;
		int x = 191;
		int y = 13 + (i * 9);
		
		auto rect = MRECT(x, y, x + w, y + h);
		bars.push_back(dynamic_pointer_cast<HorizontalBar>(addChild(make_shared<HorizontalBar>(rect, 0)).lock()));
	}
}

void TempoChangeScreen::open()
{
	for (auto& bar :bars)
		bar.lock()->Hide(true);

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

	auto events = sequencer.lock()->getActiveSequence().lock()->getTempoChangeEvents();

	if (param.length() == 2)
	{
		auto row = stoi(param.substr(1));
		{
			if (row + offset >= events.size())
			{
				offset = 0;
				ls.lock()->setFocus("a0");
			}
		}
	}
	else
	{
		offset = 0;
	}

	initVisibleEvents();

	displayInitialTempo();
	displayTempoChange0();
	displayTempoChange1();
	displayTempoChange2();
	displayTempoChangeOn();
}

void TempoChangeScreen::initVisibleEvents()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();

	for (auto& t : visibleTempoChanges)
	{
		if (t.lock())
			t.lock()->deleteObserver(this);
	}

	visibleTempoChanges = vector<weak_ptr<TempoChangeEvent>>(3);
	
	auto allTce = seq->getTempoChangeEvents();
	
	for (int i = 0; i < 3; i++)
	{
		if (i + offset < allTce.size())
			visibleTempoChanges[i] = allTce[i + offset];
		
		if (allTce.size() <= i + offset + 1)
		{
			for (int j = i + 1; j < 2; j++)
				visibleTempoChanges[j] = weak_ptr<TempoChangeEvent>();

			break;
		}
	}
}

void TempoChangeScreen::displayInitialTempo()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("initial-tempo").lock()->setText(Util::tempoString(seq->getInitialTempo()));
}

void TempoChangeScreen::displayTempoChangeOn()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	findField("tempo-change").lock()->setText(sequence->isTempoChangeOn() ? "YES" : "NO");
}

void TempoChangeScreen::displayTempoChange0()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	bars[1].lock()->Hide(false);
	
	auto tce = visibleTempoChanges[0].lock();
	a0Field.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	auto timeSig = sequence->getTimeSignature();
	int value = tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	
	b0Field.lock()->setTextPadded(value, "0");
	value = tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	c0Field.lock()->setTextPadded(value, "0");
	value = tce->getClock(timeSig.getNumerator(), timeSig.getDenominator());
	d0Field.lock()->setTextPadded(value, "0");

	string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e0Field.lock()->setText(ratioStr);

	double initialTempo = sequence->getInitialTempo();
	int ratio = tce->getRatio();

	double tempo = initialTempo * double(ratio) * 0.001;

	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f0Field.lock()->setText(Util::tempoString(tempo));
	bars[1].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void TempoChangeScreen::displayTempoChange1()
{
	int size = visibleTempoChanges.size();
	auto tce = visibleTempoChanges[1].lock();
	
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

	string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e1Field.lock()->setText(ratioStr);

	auto tempo = sequence->getInitialTempo() * (tce->getRatio() * 0.001);

	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f1Field.lock()->setText(Util::tempoString(tempo));
	bars[2].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void TempoChangeScreen::displayTempoChange2()
{
	auto tce = visibleTempoChanges[2].lock();
	
	if (!tce)
	{
		if (!visibleTempoChanges[1].lock())
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

	string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e2Field.lock()->setText(ratioStr);

	auto tempo = sequence->getInitialTempo() * tce->getRatio() * 0.001;
	
	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f2Field.lock()->setText(Util::tempoString(tempo));
	bars[3].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void TempoChangeScreen::left()
{
	init();
	
	if (param.length() == 2)
	{
		if (param[0] == 'a')
		{
			ls.lock()->setFocus("tempo-change");
			return;
		}
	}

	ScreenComponent::left();
}

void TempoChangeScreen::right()
{
	init();
	if (param.length() == 2)
	{
		if (param[0] == 'f')
		{
			ls.lock()->setFocus("initial-tempo");
			return;
		}
	}

	ScreenComponent::right();
}

void TempoChangeScreen::function(int j)
{
	ScreenComponent::function(j);
	
	auto yPos = -1;

	if (param.length() == 2)
		yPos = stoi(param.substr(1, 2));

	auto seq = sequencer.lock()->getActiveSequence().lock();

	auto tceList = seq->getTempoChangeEvents();

	switch (j)
	{
	case 1:
		if (yPos + offset >= tceList.size())
			return;
		
		if (tceList[offset + yPos].lock()->getStepNumber() == 0)
			return;
		
		seq->removeTempoChangeEvent(offset + yPos);
		seq->sortTempoChangeEvents();

		if (offset + yPos == tceList.size() - 1)
			setOffset(offset - 1);
		
		initVisibleEvents();

		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
		
		ls.lock()->setFocus("a" + to_string(yPos));
		break;
	case 2:
	{
		auto nowDetected = -1;
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
			initVisibleEvents();
			displayTempoChange0();
			displayTempoChange1();
			displayTempoChange2();

			ls.lock()->setFocus(string("a" + to_string(yPos)));
		}
		else
		{
			if (nowDetected > offset + 3 || nowDetected < offset)
				setOffset(nowDetected);

			ls.lock()->setFocus(param.substr(0, 1) + to_string(nowDetected - offset));
		}
	}
		break;
	case 3:
		openScreen("sequencer");
		break;
	case 4:
	{
		tceList = seq->getTempoChangeEvents();

		if (tceList.size() == 1)
		{
			auto tce = seq->addTempoChangeEvent().lock();
			tce->setTick(seq->getLastTick() - 1);
			tce->setStepNumber(1);
		}
		else if (tceList.size() > 1)
		{
			if (param.length() != 2)
				return;

			auto lCurrent = current.lock();
			auto lPrevious = previous.lock();

			if (yPos + offset == 0)
			{
				if (lCurrent->getTick() == 1)
					return;
				
				auto tce = seq->addTempoChangeEvent().lock();
				tce->setTick(next.lock()->getTick() - 1);
			}
			else if (yPos + offset > 0)
			{
				if (lCurrent->getTick() - 1 == lPrevious->getTick())
					return;

				auto tce = seq->addTempoChangeEvent().lock();
				tce->setTick(lCurrent->getTick() - 1);
			}
		}

		seq->sortTempoChangeEvents();
		initVisibleEvents();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();

		ls.lock()->setFocus(param);
		break;
	}
	}
}

void TempoChangeScreen::init()
{
	ScreenComponent::init();
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tceList = seq->getTempoChangeEvents();

	if (param.length() != 2)
		return;

	auto yPos = stoi(param.substr(1, 2));

	int nextPosition = yPos + offset + 1;

	if (tceList.size() > nextPosition)
		next = tceList[nextPosition];
	else
		next.reset();

	int currentPosition = yPos + offset;

	if (currentPosition + 1 > tceList.size())
		return;

	current = tceList[currentPosition];

	auto previousPosition = yPos + offset - 1;

	if (previousPosition >= 0)
		previous = tceList[previousPosition];
}

void TempoChangeScreen::turnWheel(int j)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto tceList = seq->getTempoChangeEvents();

	if (param.compare("tempo-change") == 0)
	{
		seq->setTempoChangeOn(j > 0);
		displayTempoChangeOn();
		return;
	}
	else if (param.compare("initial-tempo") == 0)
	{
		auto tce = tceList[0].lock();
		seq->setInitialTempo(seq->getInitialTempo()+ (j * 0.1));
		displayInitialTempo();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
		return;
	}
	
	auto ts = seq->getTimeSignature();

	for (int i = 0; i < 3; i++)
	{
		auto event = visibleTempoChanges[i].lock();

		if (param.compare("b" + to_string(i)) == 0)
		{
			if (j > 0)
				event->plusOneBar(ts.getNumerator(), ts.getDenominator(), next.lock().get());
			else
				event->minusOneBar(ts.getNumerator(), ts.getDenominator(), previous.lock().get());
		}
		else if (param.compare("c" + to_string(i)) == 0)
		{
			if (j > 0)
				event->plusOneBeat(ts.getDenominator(), next.lock().get());
			else
				event->minusOneBeat(ts.getDenominator(), previous.lock().get());
		}
		else if (param.compare("d" + to_string(i)) == 0)
		{
			if (j > 0)
				event->plusOneClock(next.lock().get());
			else
				event->minusOneClock(previous.lock().get());
		}
		else if (param.compare("e" + to_string(i)) == 0)
		{
			event->setRatio(event->getRatio() + j);
		}
		else if (param.compare("f" + to_string(i)) == 0)
		{
			auto seq = sequencer.lock()->getActiveSequence().lock();
			auto ratio = (event->getTempo() + j * 0.1) / seq->getInitialTempo();
			event->setRatio((int)round(ratio * 1000.0));
		}

		if (param.length() == 2 && stoi(param.substr(1)) == i)
		{
			if (i == 0)
				displayTempoChange0();
			else if (i == 1)
				displayTempoChange1();
			else if (i == 2)
				displayTempoChange2();

			break;
		}
	}
}

void TempoChangeScreen::down()
{
	init();

	auto tce1 = visibleTempoChanges[1].lock();
	auto tce2 = visibleTempoChanges[2].lock();

	if (param.compare("tempo-change") == 0)
		ls.lock()->setFocus("e0");
	else if (param.compare("initial-tempo") == 0)
		ls.lock()->setFocus("f0");
	
	if (param.length() != 2)
		return;
	
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
		setOffset(offset + 1);

		auto sequence = sequencer.lock()->getActiveSequence().lock();

		if (offset + yPos == sequence->getTempoChangeEvents().size() && param[0] != 'a')
			ls.lock()->setFocus("a2");

		return;
	}

	auto paramToFocus = param.substr(0, 1);

	if ((yPos == 0 && !tce1) || (yPos == 1 && !tce2))
		paramToFocus = "a";

	ls.lock()->setFocus(string(paramToFocus + to_string(yPos + 1)));
}

void TempoChangeScreen::up()
{
	init();

	if (param.length() != 2)
		return;

	auto yPos = stoi(param.substr(1, 2));

	if (yPos == 0)
	{
		if (offset == 0)
		{
			if (param.compare("e0") == 0)
				ls.lock()->setFocus("tempo-change");
			else if (param.compare("f0") == 0)
				ls.lock()->setFocus("initial-tempo");

			return;
		}
		setOffset(offset - 1);
		return;
	}

	ls.lock()->setFocus(param.substr(0, 1) + to_string(yPos - 1));
	return;
}

void TempoChangeScreen::setOffset(int i)
{
	if (i < 0)
		return;

	offset = i;

	initVisibleEvents();
	displayTempoChange0();
	displayTempoChange1();
	displayTempoChange2();
}
