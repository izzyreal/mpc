#include <controls/sequencer/BarCopyControls.hpp>

#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

BarCopyControls::BarCopyControls()
	: AbstractSequencerControls()
{
}

void BarCopyControls::function(int j)
{
	init();
	int firstBar;
	int lastBar;
	int copies;
	int numberOfBars;
	int afterBar;
	int firstTick;
	int lastTick;
	int firstTickOfToSeq;
	int offset;
	int segmentLengthTicks;
	auto lToSeq = toSeq.lock();
	auto lFromSeq = fromSeq.lock();
	auto uis = Mpc::instance().getUis().lock();
	switch (j) {
	case 0:
		uis->getEditSequenceGui()->setFromSq(barCopyGui->getFromSq());
		uis->getEditSequenceGui()->setToSq(barCopyGui->getToSq());
		ls.lock()->openScreen("edit");
		break;
	case 2:
		uis->getTrMoveGui()->setSq(barCopyGui->getFromSq());
		ls.lock()->openScreen("trmove");
		break;
	case 3:
		ls.lock()->openScreen("user");
		break;
	case 4:
		break;
	case 5:
		firstBar = barCopyGui->getFirstBar();
		lastBar = barCopyGui->getLastBar();
		copies = barCopyGui->getCopies();
		numberOfBars = (lastBar - firstBar + 1) * copies;
		afterBar = barCopyGui->getAfterBar();
		if (!lToSeq->isUsed()) {
			lToSeq->init(numberOfBars - 1);
		}
		else {
			lToSeq->insertBars(numberOfBars, afterBar);
		}

		int copyCounter = 0;
		for (int i = 0; i < numberOfBars; i++) {
			lToSeq->setTimeSignature(i + afterBar, lFromSeq->getNumerator(copyCounter + firstBar), lFromSeq->getDenominator(copyCounter + firstBar));
			copyCounter++;
			if (copyCounter >= copies) copyCounter = 0;
		}
		firstTick = 0;
		lastTick = 0;
		firstTickOfToSeq = 0;
		offset = 0;
		segmentLengthTicks = 0;
		for (int i = 0; i < 999; i++) {
			if (i == firstBar) break;
			firstTick += (*lFromSeq->getBarLengths())[i];
		}
		for (int i = 0; i < 999; i++) {
			lastTick += (*lFromSeq->getBarLengths())[i];
			if (i == lastBar) break;
		}

		segmentLengthTicks = lastTick - firstTick;

		for (int i = 0; i < 999; i++) {
			if (i == afterBar) break;
			firstTickOfToSeq += (*lToSeq->getBarLengths())[i];
		}
		offset = firstTickOfToSeq - firstTick;
		for (int i = 0; i < 64; i++) {
			auto t1 = lFromSeq->getTrack(i).lock();
			auto t2 = lToSeq->getTrack(i).lock();
			for (auto& e : t1->getEvents()) {
				auto event = e.lock();
				if (event->getTick() >= firstTick && event->getTick() < lastTick) {
					if (!t2->isUsed()) {
						t2->setUsed(true);
					}
					for (auto k = 0; k < copies; k++) {
						auto clone = t2->cloneEvent(event).lock();
						clone->setTick(clone->getTick() + offset + (k * segmentLengthTicks));
					}
				}
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void BarCopyControls::turnWheel(int i)
{
	init();
	auto lFromSeq = fromSeq.lock();
	auto lToSeq = toSeq.lock();
	auto lSequencer = sequencer.lock();
	if (param.compare("fromsq") == 0) {
		barCopyGui->setFromSq(barCopyGui->getFromSq() + i);
		fromSeq = lSequencer->getSequence(barCopyGui->getFromSq());
		lFromSeq = fromSeq.lock();
		if (barCopyGui->getLastBar() > lFromSeq->getLastBar())
			barCopyGui->setLastBar(lFromSeq->getLastBar(), lFromSeq->getLastBar());

	}
	else if (param.compare("tosq") == 0) {
		barCopyGui->setToSq(barCopyGui->getToSq() + i);
		toSeq = lSequencer->getSequence(barCopyGui->getToSq());
		lToSeq = toSeq.lock();
		if (barCopyGui->getAfterBar() > lToSeq->getLastBar())
			barCopyGui->setAfterBar(lToSeq->getLastBar(), lToSeq->getLastBar());

	}
	else if (param.compare("afterbar") == 0) {
		barCopyGui->setAfterBar(barCopyGui->getAfterBar() + i, lToSeq->getLastBar());
	}
	else if (param.compare("firstbar") == 0) {
		barCopyGui->setFirstBar(barCopyGui->getFirstBar() + i, lFromSeq->getLastBar());
	}
	else if (param.compare("lastbar") == 0) {
		barCopyGui->setLastBar(barCopyGui->getLastBar() + i, lFromSeq->getLastBar());
	}
	else if (param.compare("copies") == 0) {
		barCopyGui->setCopies(barCopyGui->getCopies() + i);
	}
}
