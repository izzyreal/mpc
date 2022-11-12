#include "TempoChangeScreen.hpp"

#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/HorizontalBar2.hpp>

#include <Util.hpp>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

TempoChangeScreen::TempoChangeScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "tempo-change", layerIndex)
{
	for (int i = 0; i < 3; i++)
	{
		int w = 35;
		int h = 5;
		int x = 191;
		int y = 22 + (i * 9);
		
		auto rect = MRECT(x, y, x + w, y + h);
		bars.push_back(addChildT<HorizontalBar2>(rect));
	}
}

void TempoChangeScreen::open()
{
	findLabel("initial-tempo")->setLocation(140, 10);

	for (auto& bar :bars)
		bar->Hide(true);

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

	a0Field->setAlignment(Alignment::Centered);
	a1Field->setAlignment(Alignment::Centered);
	a2Field->setAlignment(Alignment::Centered);

	auto events = sequencer->getActiveSequence()->getTempoChangeEvents();

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
	auto seq = sequencer->getActiveSequence();

	for (auto& t : visibleTempoChanges)
	{
		if (t)
			t->deleteObserver(this);
	}

	visibleTempoChanges = std::vector<std::shared_ptr<TempoChangeEvent>>(3);
	
	auto allTce = seq->getTempoChangeEvents();
	
	for (int i = 0; i < 3; i++)
	{
		if (i + offset < allTce.size())
			visibleTempoChanges[i] = allTce[i + offset];
		
		if (allTce.size() <= i + offset + 1)
		{
			for (int j = i + 1; j < 2; j++)
				visibleTempoChanges[j] = std::shared_ptr<TempoChangeEvent>();

			break;
		}
	}
}

void TempoChangeScreen::displayInitialTempo()
{
	auto seq = sequencer->getActiveSequence();
	findField("initial-tempo")->setText(Util::tempoString(seq->getInitialTempo()));
}

void TempoChangeScreen::displayTempoChangeOn()
{
	auto sequence = sequencer->getActiveSequence();
	findField("tempo-change")->setText(sequence->isTempoChangeOn() ? "YES" : "NO");
}

void TempoChangeScreen::displayTempoChange0()
{
	auto sequence = sequencer->getActiveSequence();
	bars[0]->Hide(false);
	
	auto tce = visibleTempoChanges[0];
	a0Field->setText(std::to_string(tce->getStepNumber() + 1));
	auto timeSig = sequence->getTimeSignature();
	
	int value = tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	b0Field->setTextPadded(value, "0");
	value = tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	c0Field->setTextPadded(value, "0");
	value = tce->getClock(timeSig.getDenominator());
	d0Field->setTextPadded(value, "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e0Field->setText(ratioStr);

	double initialTempo = sequence->getInitialTempo();
	int ratio = tce->getRatio();

	double tempo = initialTempo * double(ratio) * 0.001;

	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f0Field->setText(Util::tempoString(tempo));
	bars[0]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::displayTempoChange1()
{
	auto tce = visibleTempoChanges[1];
	
	if (!tce)
	{
		a1Field->setText("END");
		b1Field->Hide(true);
		c1Field->Hide(true);
		d1Field->Hide(true);
		e1Field->Hide(true);
		f1Field->Hide(true);
		b1Label->Hide(true);
		c1Label->Hide(true);
		d1Label->Hide(true);
		e1Label->Hide(true);
		f1Label->Hide(true);
		bars[1]->Hide(true);
		return;
	}
	
	b1Field->Hide(false);
	c1Field->Hide(false);
	d1Field->Hide(false);
	e1Field->Hide(false);
	f1Field->Hide(false);
	b1Label->Hide(false);
	c1Label->Hide(false);
	d1Label->Hide(false);
	e1Label->Hide(false);
	f1Label->Hide(false);
	bars[1]->Hide(false);

	a1Field->setText(std::to_string(tce->getStepNumber() + 1));
	
	auto sequence = sequencer->getActiveSequence();
	auto timeSig = sequence->getTimeSignature();

	b1Field->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c1Field->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d1Field->setTextPadded(tce->getClock(timeSig.getDenominator()), "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e1Field->setText(ratioStr);

	auto tempo = sequence->getInitialTempo() * (tce->getRatio() * 0.001);

	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f1Field->setText(Util::tempoString(tempo));
	bars[1]->setValue((tempo - 30) / 270.0);
}

void TempoChangeScreen::displayTempoChange2()
{
	auto tce = visibleTempoChanges[2];
	
	if (!tce)
	{
		if (!visibleTempoChanges[1])
		{
			a2Field->Hide(true);
		}
		else
		{
			a2Field->Hide(false);
			a2Field->setText("END");
		}

		b2Field->Hide(true);
		c2Field->Hide(true);
		d2Field->Hide(true);
		e2Field->Hide(true);
		f2Field->Hide(true);
		b2Label->Hide(true);
		c2Label->Hide(true);
		d2Label->Hide(true);
		e2Label->Hide(true);
		f2Label->Hide(true);
		bars[2]->Hide(true);
		return;
	}

	b2Field->Hide(false);
	c2Field->Hide(false);
	d2Field->Hide(false);
	e2Field->Hide(false);
	f2Field->Hide(false);
	b2Label->Hide(false);
	c2Label->Hide(false);
	d2Label->Hide(false);
	e2Label->Hide(false);
	f2Label->Hide(false);
	bars[2]->Hide(false);
	a2Field->setText(std::to_string(tce->getStepNumber() + 1));

	auto sequence = sequencer->getActiveSequence();
	auto timeSig = sequence->getTimeSignature();
	b2Field->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c2Field->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d2Field->setTextPadded(tce->getClock(timeSig.getDenominator()), "0");

    std::string ratioStr = StrUtil::TrimDecimals(tce->getRatio() * 0.1, 1);
	ratioStr = StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e2Field->setText(ratioStr);

	auto tempo = sequence->getInitialTempo() * tce->getRatio() * 0.001;
	
	if (tempo < 30.0) tempo = 30.0;
	else if (tempo > 300.0) tempo = 300.0;

	f2Field->setText(Util::tempoString(tempo));
	bars[2]->setValue((tempo - 30) / 270.0);
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

	auto seq = sequencer->getActiveSequence();

	auto tceList = seq->getTempoChangeEvents();

	switch (j)
	{
	case 1:
		if (yPos + offset >= tceList.size())
			return;
		
		if (tceList[offset + yPos]->getStepNumber() == 0)
			return;
		
		seq->removeTempoChangeEvent(offset + yPos);
		seq->sortTempoChangeEvents();

		if (offset + yPos == tceList.size() - 1)
			setOffset(offset - 1);
		
		initVisibleEvents();

		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
		
		ls.lock()->setFocus("a" + std::to_string(yPos));
		break;
	case 2:
	{
		auto nowDetected = -1;
		for (int i = 0; i < tceList.size(); i++)
		{
			if (tceList[i]->getTick() == sequencer->getTickPosition())
			{
				nowDetected = i;
				break;
			}
		}

		if (nowDetected == -1)
		{
			auto tce = seq->addTempoChangeEvent();
			tce->setTick(sequencer->getTickPosition());
			seq->sortTempoChangeEvents();
			initVisibleEvents();
			displayTempoChange0();
			displayTempoChange1();
			displayTempoChange2();

			ls.lock()->setFocus(std::string("a" + std::to_string(yPos)));
		}
		else
		{
			if (nowDetected > offset + 3 || nowDetected < offset)
				setOffset(nowDetected);

			ls.lock()->setFocus(param.substr(0, 1) + std::to_string(nowDetected - offset));
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
			auto tce = seq->addTempoChangeEvent();
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
				
				auto tce = seq->addTempoChangeEvent();
				tce->setTick(next.lock()->getTick() - 1);
			}
			else if (yPos + offset > 0)
			{
				if (lCurrent->getTick() - 1 == lPrevious->getTick())
					return;

				auto tce = seq->addTempoChangeEvent();
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
	auto seq = sequencer->getActiveSequence();
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
	
	auto seq = sequencer->getActiveSequence();
	auto tceList = seq->getTempoChangeEvents();

	if (param == "tempo-change")
	{
		seq->setTempoChangeOn(j > 0);
		displayTempoChangeOn();
		return;
	}
	else if (param == "initial-tempo")
	{
		auto tce = tceList[0];
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
		auto event = visibleTempoChanges[i];

		if (param == "b" + std::to_string(i))
		{
			if (j > 0)
                event->plusOneBar(next.lock().get());
			else
                event->minusOneBar(previous.lock().get());
		}
		else if (param == "c" + std::to_string(i))
		{
			if (j > 0)
                event->plusOneBeat(next.lock().get());
			else
                event->minusOneBeat(previous.lock().get());
		}
		else if (param == "d" + std::to_string(i))
		{
			if (j > 0)
				event->plusOneClock(next.lock().get());
			else
				event->minusOneClock(previous.lock().get());
		}
		else if (param == "e" + std::to_string(i))
		{
			event->setRatio(event->getRatio() + j);
		}
		else if (param == "f" + std::to_string(i))
		{
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

	auto tce1 = visibleTempoChanges[1];
	auto tce2 = visibleTempoChanges[2];

	if (param == "tempo-change")
		ls.lock()->setFocus("e0");
	else if (param == "initial-tempo")
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

		auto sequence = sequencer->getActiveSequence();

		if (offset + yPos == sequence->getTempoChangeEvents().size() && param[0] != 'a')
			ls.lock()->setFocus("a2");

		return;
	}

	auto paramToFocus = param.substr(0, 1);

	if ((yPos == 0 && !tce1) || (yPos == 1 && !tce2))
		paramToFocus = "a";

	ls.lock()->setFocus(std::string(paramToFocus + std::to_string(yPos + 1)));
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
			if (param == "e0")
				ls.lock()->setFocus("tempo-change");
			else if (param == "f0")
				ls.lock()->setFocus("initial-tempo");

			return;
		}
		setOffset(offset - 1);
		return;
	}

	ls.lock()->setFocus(param.substr(0, 1) + std::to_string(yPos - 1));
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
