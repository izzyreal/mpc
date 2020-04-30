#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

DirectToDiskRecorderGui::DirectToDiskRecorderGui()
{
	
	outputFolder = "DEFAULT";
}

void DirectToDiskRecorderGui::setSampleRate(int rate) {
	if (rate < 0 || rate > 2) return;
	sampleRate = rate;
	notifyObservers(string("rate"));
}

int DirectToDiskRecorderGui::getSampleRate() {
	return sampleRate;
}

int DirectToDiskRecorderGui::getTime0()
{
    return time0;
}

void DirectToDiskRecorderGui::setTime0(int time0)
{
    this->time0 = time0;
    if(time0 > time1) time1 = time0;
    setChanged();
    notifyObservers(string("time"));
}

int DirectToDiskRecorderGui::getTime1()
{
    return time1;
}

void DirectToDiskRecorderGui::setTime1(int time1)
{
    this->time1 = time1;
    if(time1 < time0)
        time0 = time1;

    setChanged();
    notifyObservers(string("time"));
}

void DirectToDiskRecorderGui::setRecord(int i)
{
    if(i < 0 || i > 4)
        return;

    record = i;
    setChanged();
    notifyObservers(string("record"));
}

int DirectToDiskRecorderGui::getRecord()
{
    return record;
}

void DirectToDiskRecorderGui::setSq(int i)
{
	if (i < 0 || i > 99) return;
	sq = i;
	setChanged();
	notifyObservers(string("sq"));
	setTime0(0);
	auto s = Mpc::instance().getSequencer().lock()->getSequence(sq).lock();
	if (s->isUsed()) {
		setTime1(s->getLastTick());
	}
	else {
		setTime1(0);
	}
}

int DirectToDiskRecorderGui::getSq()
{
	return sq;
}

void DirectToDiskRecorderGui::setSong(int i)
{
    if(i < 0 || i > 4) return;
    song = i;
    setChanged();
    notifyObservers(string("song"));
}

int DirectToDiskRecorderGui::getSong()
{
    return song;
}

void DirectToDiskRecorderGui::setOutputFolder(string s)
{
	//outputFolder = s->toUpperCase();
	setChanged();
	notifyObservers(string("outputfolder"));
}

string DirectToDiskRecorderGui::getOutputfolder()
{
    return outputFolder;
}

void DirectToDiskRecorderGui::setOffline(bool b)
{
    offline = b;
    setChanged();
    notifyObservers(string("offline"));
}

bool DirectToDiskRecorderGui::isOffline()
{
    return offline;
}

void DirectToDiskRecorderGui::setSplitLR(bool b)
{
    splitLR = b;
    setChanged();
    notifyObservers(string("splitlr"));
}

bool DirectToDiskRecorderGui::isSplitLR()
{
	return splitLR;
}