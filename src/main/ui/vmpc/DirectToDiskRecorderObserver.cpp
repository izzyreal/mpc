#include <ui/vmpc/DirectToDiskRecorderObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Song.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

DirectToDiskRecorderObserver::DirectToDiskRecorderObserver() 
{
	
	recordNames = vector<string>{ "SEQUENCE", "LOOP", "CUSTOM RANGE", "SONG", "JAM" };
	d2dRecorderGui = Mpc::instance().getUis().lock()->getD2DRecorderGui();
	d2dRecorderGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	recordField = ls->lookupField("record");
	sqField = ls->lookupField("sq");
	songField = ls->lookupField("song");
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	outputFolderField = ls->lookupField("outputfolder");
	offlineField = ls->lookupField("offline");
	rateField = ls->lookupField("rate");
	splitLRField = ls->lookupField("splitlr");
	displayRecord();
	displaySq();
	displaySong();
	displayTime();
	displayOutputFolder();
	displayOffline();
	displaySplitLR();
	displayRate();
}

void DirectToDiskRecorderObserver::displayRate() {
	bool offline = d2dRecorderGui->isOffline();
	rateField.lock()->Hide(!offline);
	Mpc::instance().getLayeredScreen().lock()->lookupLabel("rate").lock()->Hide(!offline);
	if (!offline) return;
	vector<string> rates{ "44.1", "48.0", "88.2" };
	string rate = Util::replaceDotWithSmallSpaceDot(rates[d2dRecorderGui->getSampleRate()]);
	rateField.lock()->setText(rate);
}

void DirectToDiskRecorderObserver::displaySong()
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	songField.lock()->Hide(d2dRecorderGui->getRecord() != 3);
	ls->lookupLabel("song").lock()->Hide(d2dRecorderGui->getRecord() != 3);
	if (d2dRecorderGui->getRecord() != 3) return;

	auto song = d2dRecorderGui->getSong();
	songField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(song + 1), "0", 2) + "-" + Mpc::instance().getSequencer().lock()->getSong(song).lock()->getName());
}

void DirectToDiskRecorderObserver::displayOffline()
{
    offlineField.lock()->setText(d2dRecorderGui->isOffline() ? "YES" : "NO");
}

void DirectToDiskRecorderObserver::displaySplitLR()
{
	splitLRField.lock()->setText(d2dRecorderGui->isSplitLR() ? "YES" : "NO");
}

void DirectToDiskRecorderObserver::displayOutputFolder()
{
    outputFolderField.lock()->setText(d2dRecorderGui->getOutputfolder());
}

void DirectToDiskRecorderObserver::displayRecord()
{
	recordField.lock()->setText(recordNames[d2dRecorderGui->getRecord()]);
}

void DirectToDiskRecorderObserver::displaySq()
{
	sqField.lock()->Hide(!(d2dRecorderGui->getRecord() >= 0 && d2dRecorderGui->getRecord() <= 2));
	auto ls = Mpc::instance().getLayeredScreen().lock();
	ls->lookupLabel("sq").lock()->Hide(!(d2dRecorderGui->getRecord() >= 0 && d2dRecorderGui->getRecord() <= 2));
	auto seq = d2dRecorderGui->getSq();
	sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(seq + 1), "0", 2) + "-" + Mpc::instance().getSequencer().lock()->getSequence(seq).lock()->getName());
}

void DirectToDiskRecorderObserver::displayTime()
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	for (int i = 0; i < 6; i++) {
		ls->lookupField("time" + to_string(i)).lock()->Hide(d2dRecorderGui->getRecord() != 2);
		ls->lookupLabel("time" + to_string(i)).lock()->Hide(d2dRecorderGui->getRecord() != 2);
	}
	if (d2dRecorderGui->getRecord() != 2) {
		return;
	}
	auto sequence = Mpc::instance().getSequencer().lock()->getSequence(d2dRecorderGui->getSq()).lock();
	time0Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBarNumber(sequence.get(), d2dRecorderGui->getTime0() + 1), "0");
	time1Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBeatNumber(sequence.get(), d2dRecorderGui->getTime0() + 1), "0");
	time2Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getClockNumber(sequence.get(), d2dRecorderGui->getTime0()), "0");
	time3Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBarNumber(sequence.get(), d2dRecorderGui->getTime1() + 1), "0");
	time4Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBeatNumber(sequence.get(), d2dRecorderGui->getTime1() + 1), "0");
	time5Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getClockNumber(sequence.get(), d2dRecorderGui->getTime1()), "0");
}

void DirectToDiskRecorderObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	Mpc::instance().getLayeredScreen().lock()->lookupLabel("sq").lock()->SetDirty();
	string param = nonstd::any_cast<string>(a);
	if (param.compare("rate") == 0) {
		displayRate();
	}
	else if (param.compare("offline") == 0) {
		displayOffline();
		displayRate();
	}
	else if (param.compare("splitlr") == 0) {
		displaySplitLR();
	}
	else if (param.compare("outputfolder") == 0) {
		displayOutputFolder();
	}
	else if (param.compare("time") == 0) {
		displayTime();
	}
	else if (param.compare("record") == 0) {
		displayRecord();
		displaySq();
		displaySong();
		displayTime();
	}
	else if (param.compare("sq") == 0) {
		displaySq();
	}
	else if (param.compare("song") == 0) {
		displaySong();
	}
}

DirectToDiskRecorderObserver::~DirectToDiskRecorderObserver() {
	d2dRecorderGui->deleteObserver(this);
}
